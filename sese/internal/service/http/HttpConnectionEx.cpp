#include <sese/io/InputBufferWrapper.h>
#include <sese/util/Endian.h>
#include <sese/net/http/HPackUtil.h>
#include <sese/net/http/HttpConverter.h>
#include <sese/text/StringBuilder.h>
#include <sese/Util.h>
// #include <sese/Log.h>

#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/service/http/HttpConnectionEx.h>
#include <sese/internal/service/http/HttpServiceImpl.h>

sese::internal::service::http::HttpStream::HttpStream(uint32_t id, uint32_t write_window_size, sese::net::IPAddress::Ptr addr) noexcept
    : Handleable(),
      id(id),
      endpoint_window_size(write_window_size),
      expect_length(0),
      real_length(0) {
    using namespace sese::net::http;
    request.setVersion(HttpVersion::VERSION_2);
    response.setVersion(HttpVersion::VERSION_2);
    remote_address = addr;
}

void sese::internal::service::http::HttpStream::prepareRange() {
    file->setSeek(static_cast<int64_t>(range_iterator->begin), sese::io::Seek::BEGIN); // NOLINT
    expect_length = range_iterator->len;
    real_length = 0;
    ++range_iterator;
}

sese::internal::service::http::HttpConnectionEx::HttpConnectionEx(
        const std::shared_ptr<HttpServiceImpl> &service,
        asio::io_context &io_context,
        const sese::net::IPAddress::Ptr &addr
)
    : timer(io_context),
      remote_address(addr),
      service(service) {
}

void sese::internal::service::http::HttpConnectionEx::close(uint32_t id) {
    streams.erase(id);
    closed_streams.emplace(id);
}

void sese::internal::service::http::HttpConnectionEx::readMagic() {
    readBlock(temp_buffer, 24, [this](const asio::error_code &ec) {
        if (ec) {
            return;
        }
        // 魔数错误
        if (0 != strncmp(temp_buffer, sese::net::http::MAGIC_STRING, 24)) {
            return;
        }
        writeSettingsFrame();
    });
}

void sese::internal::service::http::HttpConnectionEx::readFrameHeader() {
    using namespace sese::net::http;
    readBlock(temp_buffer, 9, [this](const asio::error_code &ec) {
        if (ec) {
            return;
        }
        memset(&frame, 0, sizeof(frame));
        memcpy(reinterpret_cast<char *>(&frame.length) + 1, temp_buffer + 0, 3);
        memcpy(&frame.type, temp_buffer + 3, 1);
        memcpy(&frame.flags, temp_buffer + 4, 1);
        memcpy(&frame.ident, temp_buffer + 5, 4);
        frame.length = FromBigEndian32(frame.length);
        frame.ident = FromBigEndian32<uint32_t>(frame.ident);
        frame.ident &= 0x7fffffff;

        if (frame.length > endpoint_max_frame_size) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "shutdown", true);
            return;
        }

        readBlock(temp_buffer, frame.length, [this](const asio::error_code &ec0) {
            if (ec0) {
                return;
            }
            handleFrameHeader();
        });
    });
}

void sese::internal::service::http::HttpConnectionEx::handleFrameHeader() {
    using namespace sese::net::http;
    auto iterator = streams.find(frame.ident);
    // CONTINUATION 帧不连续
    // 判断前序帧2
    if (iterator != streams.end()) {
        if (frame.type != FRAME_TYPE_CONTINUATION &&
            iterator->second->continue_type == FRAME_TYPE_CONTINUATION &&
            iterator->second->end_headers == false) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }
    }
    switch (frame.type) {
        case FRAME_TYPE_SETTINGS: {
            auto code = handleSettingsFrame();
            if (code == UINT8_MAX) {
                // Recv ACK
                readFrameHeader();
            } else if (code == 0) {
                // All be OK
                writeAckFrame();
            }
            break;
        }
        case FRAME_TYPE_WINDOW_UPDATE: {
            handleWindowUpdate();
            break;
        }
        case FRAME_TYPE_GOAWAY: {
            handleGoawayFrame();
            break;
        }
        // 对于服务器而言，只需要处理 HEADERS 后的 CONTINUATION
        // 判断前序帧1
        case FRAME_TYPE_CONTINUATION: {
            if (frame.ident == 0) {
                writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
                break;
            }
            if (iterator == streams.end()) {
                writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                break;
            }
            if (iterator->second->continue_type != FRAME_TYPE_HEADERS &&
                iterator->second->continue_type != FRAME_TYPE_CONTINUATION) {
                writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                break;
            }
            if (iterator->second->end_headers) {
                writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                break;
            }
        }
        case FRAME_TYPE_HEADERS: {
            handleHeadersFrame();
            break;
        }
        case FRAME_TYPE_DATA: {
            handleDataFrame();
            break;
        }
        case FRAME_TYPE_PRIORITY: {
            handlePriorityFrame();
            break;
        }
        case FRAME_TYPE_RST_STREAM: {
            handleRstStreamFrame();
            break;
        }
        case FRAME_TYPE_PING: {
            handlePingFrame();
            break;
        }
        default: {
            writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "unknown frame type");
            break;
        }
    }
}

uint8_t sese::internal::service::http::HttpConnectionEx::handleSettingsFrame() {
    using namespace sese::net::http;
    if (frame.ident != 0) {
        return GOAWAY_PROTOCOL_ERROR;
    }
    if (frame.flags & SETTINGS_FLAGS_ACK) {
        if (frame.length) {
            return GOAWAY_FRAME_SIZE_ERROR;
        }
        expect_ack = false;
        return UINT8_MAX;
    }
    if (frame.length % 6) {
        return GOAWAY_FRAME_SIZE_ERROR;
    }
    char buffer[6];
    auto ident = reinterpret_cast<uint16_t *>(&buffer[0]);
    auto value = reinterpret_cast<uint32_t *>(&buffer[2]);
    auto input = io::InputBufferWrapper(temp_buffer, frame.length);

    while (input.read(buffer, 6) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case SETTINGS_HEADER_TABLE_SIZE:
                this->req_dynamic_table.resize(*value);
                this->header_table_size = *value;
                break;
            case SETTINGS_MAX_CONCURRENT_STREAMS:
                this->max_concurrent_stream = *value == 0 ? 100 : *value;
                break;
            case SETTINGS_MAX_FRAME_SIZE:
                if (*value > 16777215 || *value < 16384) {
                    return GOAWAY_PROTOCOL_ERROR;
                }
                this->endpoint_max_frame_size = *value;
                this->max_frame_size = std::min(this->endpoint_max_frame_size, MAX_FRAME_SIZE);
                break;
            case SETTINGS_ENABLE_PUSH:
                if (*value <= 1) {
                    this->enable_push = *value;
                } else {
                    return GOAWAY_PROTOCOL_ERROR;
                }
                break;
            case SETTINGS_MAX_HEADER_LIST_SIZE:
                this->max_header_list_size = *value;
                req_dynamic_table.resize(max_header_list_size);
                resp_dynamic_table.resize(max_header_list_size);
                break;
            case SETTINGS_INITIAL_WINDOW_SIZE:
                if (accept_stream_count) {
                    return GOAWAY_FLOW_CONTROL_ERROR;
                }
                if (*value > 2147483647) {
                    return GOAWAY_FLOW_CONTROL_ERROR;
                }
                this->endpoint_init_window_size = *value;
                break;
            default:
                break;
        }
    }
    return 0;
}

void sese::internal::service::http::HttpConnectionEx::handleWindowUpdate() {
    using namespace sese::net::http;
    auto data = reinterpret_cast<uint32_t *>(temp_buffer);
    if (frame.length != 4) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_FRAME_SIZE_ERROR, "");
        return;
    }
    auto i = FromBigEndian32(*data);
    if (i == 0) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    if (frame.ident == 0) {
        if (sese::isAdditionOverflow<
                    int32_t>(static_cast<int32_t>(endpoint_init_window_size), static_cast<int32_t>(i))) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_FLOW_CONTROL_ERROR, "");
            return;
        }
        endpoint_window_size += i;
    } else {
        auto iterator = streams.find(frame.ident);
        if (iterator != streams.end()) {
            auto stream = iterator->second;
            stream->continue_type = frame.type;
            if (sese::isAdditionOverflow<int32_t>(static_cast<int32_t>(stream->endpoint_window_size), static_cast<int32_t>(i))) {
                writeRstStreamFrame(frame.ident, 0, GOAWAY_FLOW_CONTROL_ERROR);
                return;
            }
            stream->endpoint_window_size += i;
        }
    }

    readFrameHeader();
}

void sese::internal::service::http::HttpConnectionEx::handleGoawayFrame() {
    using namespace sese::net::http;
    if (frame.ident != 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    // if (closed_streams.contains(frame.ident)) {
    //     writeGoawayFrame(frame.ident, 0, GOAWAY_STREAM_CLOSED, "");
    //     return;
    // }

    auto iterator = streams.find(frame.ident);
    if (iterator != streams.end()) {
        // writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        iterator->second->continue_type = frame.type;
        // return;
    }

    uint32_t latest_stream;
    memcpy(&latest_stream, temp_buffer, sizeof(latest_stream));
    latest_stream = FromBigEndian32(latest_stream);
    uint32_t error_code;
    memcpy(&error_code, temp_buffer + 4, sizeof(latest_stream));
    error_code = FromBigEndian32(error_code);
    if (frame.length - 8) {
        auto msg = std::string(temp_buffer + 8, frame.length - 8);
        // SESE_WARN("FAILED: LS {} CODE {} MSG {}", latest_stream, error_code, msg);
        if (msg == "shutdown") {
            return;
        }
    } else {
        // SESE_WARN("FAILED: LS {} CODE {}", latest_stream, error_code);
    }
    readFrameHeader();
}

void sese::internal::service::http::HttpConnectionEx::handleHeadersFrame() {
    using namespace sese::net::http;
    // if (expect_ack) {
    //     writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "expect ack");
    //     return;
    // }
    if (frame.ident == 0 ||
        frame.ident % 2 != 1) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    if (closed_streams.contains(frame.ident)) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_STREAM_CLOSED, "");
        return;
    }

    HttpStream::Ptr stream;
    auto iterator = streams.find(frame.ident);
    if (frame.type == FRAME_TYPE_HEADERS) {
        if (iterator == streams.end()) {
            if (frame.ident < latest_stream_ident) {
                writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
                return;
            }
            if (streams.size() > MAX_CONCURRENT_STREAMS) {
                writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "shutdown", true);
                return;
            }
            stream = std::make_shared<HttpStream>(frame.ident, endpoint_init_window_size, remote_address);
            streams[frame.ident] = stream;
            accept_stream_count += 1;
            latest_stream_ident = frame.ident;
        } else {
            stream = iterator->second;
            if (stream->end_headers) {
                writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                return;
            }
        }
    } else {
        if (iterator == streams.end()) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }
        stream = iterator->second;
    }


    uint8_t offset = 0;
    uint8_t padded = 0;
    stream->continue_type = frame.type;
    if (frame.flags & FRAME_FLAG_PADDED) {
        padded = temp_buffer[0];
        offset += 1;
    }
    if (frame.flags & FRAME_FLAG_PRIORITY) {
        uint32_t dependency;
        memcpy(&dependency, temp_buffer + offset, 4);
        dependency = FromBigEndian32(dependency);
        offset += 4;
        uint8_t priority = temp_buffer[offset]; // NOLINT
        offset += 1;

        if (dependency == frame.ident) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }
    }

    if (padded > frame.length) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    stream->temp_buffer.write(temp_buffer + offset, frame.length - padded - offset);

    if (frame.flags & FRAME_FLAG_END_HEADERS) {
        stream->end_headers = true;
    }
    if (frame.flags & FRAME_FLAG_END_STREAM) {
        stream->end_stream = true;
    }

    if (stream->end_headers) {
        auto rt = HPackUtil::decode(&stream->temp_buffer, stream->temp_buffer.getReadableSize(), req_dynamic_table, stream->request, false, true, header_table_size);
        stream->temp_buffer.freeCapacity();
        if (rt) {
            writeGoawayFrame(frame.ident, 0, rt, "");
            return;
        }
        // if (stream->request.exist("trailer") ||
        //     stream->request.exist("te")) {
        //     writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        //     return;
        // }

        rt = HttpConverter::convertFromHttp2(&stream->request);
        if (!rt) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }

        auto service = this->service.lock();
        service->handleFilter(stream);

        if (stream->end_stream) {
            handleRequest(stream);

            HttpConverter::convert2Http2(&stream->response);
            Header header;
            HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

            handleWrite();
        } else {
            readFrameHeader();
        }
    } else {
        readFrameHeader();
    }
}

void sese::internal::service::http::HttpConnectionEx::handleDataFrame() {
    using namespace sese::net::http;
    if (expect_ack) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "expect ack");
        return;
    }

    if (frame.ident == 0 ||
        frame.ident % 2 != 1) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    if (closed_streams.contains(frame.ident)) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_STREAM_CLOSED, "");
        return;
    }

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    auto stream = iterator->second;
    if (stream->end_stream) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
    }

    stream->continue_type = frame.type;

    if (frame.length > window_size ||
        frame.length > stream->window_size) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_FLOW_CONTROL_ERROR, "");
        return;
    }

    window_size -= frame.length;
    stream->window_size -= frame.length;

    if (window_size < INIT_WINDOW_SIZE / 2) {
        writeWindowUpdateFrame(0, 0, INIT_WINDOW_SIZE);
        window_size += INIT_WINDOW_SIZE;
    }
    if (stream->window_size < INIT_WINDOW_SIZE / 2) {
        writeWindowUpdateFrame(stream->id, 0, INIT_WINDOW_SIZE);
        stream->window_size += INIT_WINDOW_SIZE;
    }

    if (frame.flags & FRAME_FLAG_PADDED) {
        uint8_t padded = temp_buffer[0];
        if (padded > frame.length) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }

        if (stream->conn_type != ConnType::FILTER) {
            stream->request.getBody().write(temp_buffer + 1, frame.length - padded - 1);
        }
    } else {
        if (stream->conn_type != ConnType::FILTER) {
            stream->request.getBody().write(temp_buffer, frame.length);
        }
    }

    if (frame.flags & FRAME_FLAG_END_STREAM) {
        if (stream->conn_type != ConnType::FILTER) {
            // 被拦截的情况下不会读取 body，自然也不需要验证长度
            if (stream->request.exist("content-length")) {
                auto content_length = toInteger(stream->request.get("content-length"));
                if (content_length != stream->request.getBody().getReadableSize()) {
                    writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                    return;
                }
            }
        }
        if (stream->request.exist("te") ||
            stream->request.exist("trailer")) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }

        handleRequest(stream);

        HttpConverter::convert2Http2(&stream->response);
        Header header;
        HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

        handleWrite();
    } else {
        readFrameHeader();
    }
}

void sese::internal::service::http::HttpConnectionEx::handleRstStreamFrame() {
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    if (frame.length != 4) {
        writeGoawayFrame(0, 0, GOAWAY_FRAME_SIZE_ERROR, "");
        return;
    }

    if (closed_streams.contains(frame.ident)) {
        // writeGoawayFrame(frame.ident, 0, GOAWAY_STREAM_CLOSED, "");
        readFrameHeader();
        return;
    }

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    auto stream = iterator->second;
    close(stream->id);

    uint32_t code;
    memcpy(temp_buffer, &code, 4);
    code = FromBigEndian32(code);

    readFrameHeader();
}


void sese::internal::service::http::HttpConnectionEx::handlePriorityFrame() {
    using namespace sese::net::http;
    if (frame.ident == 0 ||
        frame.ident % 2 != 1) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    if (frame.length != 5) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_FRAME_SIZE_ERROR, "");
        return;
    }

    HttpStream::Ptr stream;
    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        stream = std::make_shared<HttpStream>(frame.ident, endpoint_init_window_size, remote_address);
        streams[frame.ident] = stream;
        accept_stream_count += 1;
    } else {
        stream = iterator->second;
    }
    stream->continue_type = frame.type;

    // 读取负载但不处理
    uint8_t exclusive_flag = 0; // NOLINT
    uint32_t stream_dependency = 0;
    uint8_t weight = 0;

    memcpy(&stream_dependency, temp_buffer, 4);
    stream_dependency = FromBigEndian32(stream_dependency);
    exclusive_flag = (stream_dependency & 0x80000000) >> 31; // NOLINT
    stream_dependency &= 0x7FFFFFFF;
    memcpy(&weight, temp_buffer + 4, 1);

    if (stream_dependency == frame.ident) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    readFrameHeader();
}

void sese::internal::service::http::HttpConnectionEx::handlePingFrame() {
    using namespace sese::net::http;
    if (frame.ident != 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "", true);
        return;
    }
    if (frame.length != 8) {
        writeGoawayFrame(0, 0, GOAWAY_FRAME_SIZE_ERROR, "");
        return;
    }
    if (frame.flags & SETTINGS_FLAGS_ACK) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "unexpected ping with ack");
        return;
    }

    auto frame = std::make_unique<Http2Frame>(8);
    frame->type = FRAME_TYPE_PING;
    frame->length = 8;
    frame->ident = 0;
    frame->flags = SETTINGS_FLAGS_ACK;
    frame->buildFrameHeader();
    memcpy(frame->getFrameContentBuffer(), temp_buffer, 8);

    pre_vector.push_back(std::move(frame));
    handleWrite();
}

void sese::internal::service::http::HttpConnectionEx::handleRequest(const HttpStream::Ptr &stream) { // NOLINT
    auto serv = service.lock();
    assert(serv);
    serv->handleRequest(stream);
    stream->do_response = true;
}

void sese::internal::service::http::HttpConnectionEx::handleWrite() {
    if (!is_read) {
        this->readFrameHeader();
    }

    if (is_write) {
        return;
    }

    for (auto &&current = streams.begin(); current != streams.end();) {
        auto stream = current->second;
        // 流未进入响应状态
        if (!stream->do_response) {
            ++current;
            continue;
        }
        // 常规响应
        if (stream->conn_type == ConnType::NONE ||
            stream->conn_type == ConnType::FILTER ||
            stream->conn_type == ConnType::CONTROLLER) {
            if (!stream->temp_buffer.eof()) {
                if (writeHeadersFrame(stream)) {
                    current = streams.erase(current);
                    closed_streams.emplace(stream->id);
                } else {
                    ++current;
                }
                continue;
            }
            if (!stream->response.getBody().eof()) {
                if (writeDataFrame4Body(stream)) {
                    current = streams.erase(current);
                    closed_streams.emplace(stream->id);
                } else {
                    ++current;
                }
                continue;
            }

            current = streams.erase(current);
            closed_streams.emplace(stream->id);
        }
        // 文件下载
        else if (stream->conn_type == ConnType::FILE_DOWNLOAD) {
            if (!stream->temp_buffer.eof()) {
                writeHeadersFrame(stream, false); // NOLINT
                ++current;
                continue;
            }
            // 单区间文件
            if (stream->ranges.size() == 1) {
                if (writeDataFrame4SingleRange(stream)) {
                    current = streams.erase(current);
                    closed_streams.emplace(stream->id);
                } else {
                    ++current;
                }
            }
            // 多区间文件
            else if (stream->ranges.size() > 1) {
                if (writeDataFrame4Ranges(stream)) {
                    current = streams.erase(current);
                    closed_streams.emplace(stream->id);
                } else {
                    ++current;
                }
            }
        } else {
            ++current;
        }
    }

    if (!pre_vector.empty()) {
        vector.clear();
        vector.swap(pre_vector);
        asio_buffers.clear();
        asio_buffers.reserve(vector.size());
        for (auto &&item: vector) {
            asio_buffers.emplace_back(asio::buffer(item->getFrameBuffer(), item->getFrameLength()));
        }
        writeBlocks(asio_buffers, [conn = getPtr()](const asio::error_code &ec) {
            if (ec) {
                return;
            }
            conn->handleWrite();
        });
    }
}

void sese::internal::service::http::HttpConnectionEx::writeGoawayFrame(
        uint32_t latest_stream_id,
        uint8_t flags,
        uint32_t error_code,
        const std::string &msg,
        bool once
) {
    using namespace sese::net::http;
    auto frame = std::make_unique<Http2Frame>(msg.length() + 8);
    frame->type = FRAME_TYPE_GOAWAY;
    frame->flags = flags;
    frame->ident = 0;
    frame->length = static_cast<uint32_t>(msg.length() + 8);
    frame->buildFrameHeader();
    latest_stream_id = ToBigEndian32(latest_stream_id);
    error_code = ToBigEndian32(error_code);
    memcpy(frame->getFrameContentBuffer() + 0, &latest_stream_id, 4);
    memcpy(frame->getFrameContentBuffer() + 4, &error_code, 4);
    if (once) {
        auto buf = frame->getFrameBuffer();
        auto len = frame->getFrameLength();
        writeBlock(buf, len, [conn = getPtr(), f = std::shared_ptr(std::move(frame))](const asio::error_code &) {
        });
    } else {
        pre_vector.push_back(std::move(frame));
        handleWrite();
    }
}

void sese::internal::service::http::HttpConnectionEx::writeRstStreamFrame(
        uint32_t stream_id,
        uint8_t flags,
        uint32_t error_code,
        bool once
) {
    using namespace sese::net::http;
    auto frame = std::make_unique<Http2Frame>(4);
    frame->type = FRAME_TYPE_RST_STREAM;
    frame->flags = flags;
    frame->ident = stream_id;
    frame->length = 4;
    frame->buildFrameHeader();
    error_code = ToBigEndian32(error_code);
    memcpy(frame->getFrameContentBuffer(), &error_code, 4);
    if (once) {
        auto buf = frame->getFrameBuffer();
        auto len = frame->getFrameLength();
        writeBlock(buf, len, [conn = getPtr(), f = std::shared_ptr(std::move(frame))](const asio::error_code &) {
        });
    } else {
        pre_vector.push_back(std::move(frame));
        handleWrite();
    }
}


void sese::internal::service::http::HttpConnectionEx::writeSettingsFrame() {
    using namespace sese::net::http;
    std::vector<std::pair<uint16_t, uint32_t>> values = {
            {SETTINGS_INITIAL_WINDOW_SIZE, INIT_WINDOW_SIZE},
            {SETTINGS_MAX_FRAME_SIZE, MAX_FRAME_SIZE},
            {SETTINGS_HEADER_TABLE_SIZE, HEADER_TABLE_SIZE},
            {SETTINGS_MAX_CONCURRENT_STREAMS, MAX_CONCURRENT_STREAMS}
    };

    auto frame = std::make_unique<Http2Frame>(values.size() * 6);
    frame->length = static_cast<uint32_t>(values.size() * 6);
    frame->type = FRAME_TYPE_SETTINGS;
    frame->flags = 0;
    frame->ident = 0;
    frame->buildFrameHeader();

    auto buffer = frame->getFrameContentBuffer();
    int pos = 0;
    for (auto [key, value]: values) {
        key = ToBigEndian16(key);
        value = ToBigEndian32(value);
        memcpy(buffer + pos, &key, sizeof(key));
        pos += sizeof(key);
        memcpy(buffer + pos, &value, sizeof(value));
        pos += sizeof(value);
    }

    expect_ack = true;
    pre_vector.push_back(std::move(frame));
    handleWrite();
}

void sese::internal::service::http::HttpConnectionEx::writeAckFrame() {
    auto frame = std::make_unique<sese::net::http::Http2Frame>(0);
    frame->type = sese::net::http::FRAME_TYPE_SETTINGS;
    frame->flags = sese::net::http::SETTINGS_FLAGS_ACK;
    frame->buildFrameHeader();
    pre_vector.push_back(std::move(frame));
    handleWrite();
}

void sese::internal::service::http::HttpConnectionEx::writeWindowUpdateFrame(uint32_t stream_id, uint8_t flags, uint32_t window_size) {
    auto frame = std::make_unique<sese::net::http::Http2Frame>(4);
    frame->type = sese::net::http::FRAME_TYPE_WINDOW_UPDATE;
    frame->length = 4;
    frame->ident = stream_id;
    frame->flags = flags;
    frame->buildFrameHeader();
    window_size = ToBigEndian32(window_size);
    memcpy(frame->getFrameContentBuffer(), &window_size, 4);
    pre_vector.push_back(std::move(frame));
    handleWrite();
}

bool sese::internal::service::http::HttpConnectionEx::writeHeadersFrame(const HttpStream::Ptr &stream, bool verify_end_stream) {
    auto result = false;
    auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
    frame->ident = stream->id;
    auto len = stream->temp_buffer.read(frame->getFrameContentBuffer(), max_frame_size);
    frame->type = sese::net::http::FRAME_TYPE_HEADERS;
    frame->length = static_cast<uint32_t>(len);
    if (stream->temp_buffer.eof()) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_HEADERS;
    }
    if (verify_end_stream && stream->response.getBody().eof()) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_STREAM;
        result = true;
    }
    frame->buildFrameHeader();
    pre_vector.push_back(std::move(frame));
    return result;
}

bool sese::internal::service::http::HttpConnectionEx::writeDataFrame4Body(const HttpStream::Ptr &stream) {
    // 窗口大小不足
    if (endpoint_window_size == 0 ||
        stream->endpoint_window_size == 0) {
        return false;
    }
    auto result = false;
    auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
    frame->ident = stream->id;
    auto remind = std::min({endpoint_window_size, stream->endpoint_window_size, max_frame_size});
    auto len = stream->response.getBody().read(frame->getFrameContentBuffer(), remind);
    frame->type = sese::net::http::FRAME_TYPE_DATA;
    frame->length = static_cast<uint32_t>(len);
    if (stream->response.getBody().eof()) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_STREAM;
        result = true;
    }
    frame->buildFrameHeader();
    pre_vector.push_back(std::move(frame));
    return result;
}

bool sese::internal::service::http::HttpConnectionEx::writeDataFrame4SingleRange(const HttpStream::Ptr &stream) {
    // 窗口大小不足
    if (endpoint_window_size == 0 ||
        stream->endpoint_window_size == 0) {
        return false;
    }

    // 初次准备
    if (stream->expect_length == 0) {
        stream->prepareRange();
    }

    auto result = false;
    auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
    frame->ident = stream->id;
    size_t remind = std::min({endpoint_window_size, stream->endpoint_window_size, max_frame_size});
    auto l = std::min<size_t>(stream->expect_length - stream->real_length, remind);
    stream->real_length += l;
    stream->file->read(frame->getFrameContentBuffer(), l);
    frame->length = static_cast<uint32_t>(l);
    if (stream->real_length >= stream->expect_length) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_STREAM;
        result = true;
    }
    frame->buildFrameHeader();
    pre_vector.push_back(std::move(frame));
    return result;
}


bool sese::internal::service::http::HttpConnectionEx::writeDataFrame4Ranges(const HttpStream::Ptr &stream) {
    // 此函数的所有直接返回 false 均为窗口大小不足所导致
    // 通过 result 返回均为正常处理
    if (endpoint_window_size == 0 ||
        stream->endpoint_window_size == 0) {
        return false;
    }
    auto result = false;
    size_t remind = std::min({endpoint_window_size, stream->endpoint_window_size, max_frame_size});
    if (stream->real_length >= stream->expect_length) {
        if (stream->range_iterator == stream->ranges.begin()) {
            // 首个区间
            auto subheader = std::string("--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + stream->content_type + "\r\n" +
                             "content-range: " + stream->range_iterator->toString(stream->filesize) + "\r\n\r\n";
            if (subheader.length() > remind) {
                return false;
            }
            remind -= subheader.length();
            stream->prepareRange();
            writeSubheaderAndData(stream, subheader, remind);
        } else if (stream->range_iterator == stream->ranges.end()) {
            // 最后一个区间已结束
            auto end_boundary = std::string("\r\n--") + HTTPD_BOUNDARY + "--\r\n";
            if (end_boundary.length() > remind) {
                return false;
            }
            auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
            frame->ident = stream->id;
            frame->type = sese::net::http::FRAME_TYPE_DATA;
            frame->length = static_cast<uint32_t>(end_boundary.length());
            frame->flags |= sese::net::http::FRAME_FLAG_END_STREAM;
            frame->buildFrameHeader();
            memcpy(frame->getFrameContentBuffer(), end_boundary.data(), end_boundary.length());
            pre_vector.push_back(std::move(frame));
            return true;
        } else {
            // 中间的区间和最后一个区间
            auto subheader = std::string("\r\n--") + HTTPD_BOUNDARY + "\r\n" +
                             "content-type: " + stream->content_type + "\r\n" +
                             "content-range: " + stream->range_iterator->toString(stream->filesize) + "\r\n\r\n";
            if (subheader.length() > remind) {
                return false;
            }
            remind -= subheader.length();
            stream->prepareRange();
            writeSubheaderAndData(stream, subheader, remind);
        }
    } else {
        auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
        frame->ident = stream->id;
        auto l = std::min<size_t>(stream->expect_length - stream->real_length, remind);
        stream->real_length += l;
        stream->file->read(frame->getFrameContentBuffer(), l);
        frame->buildFrameHeader();
        pre_vector.push_back(std::move(frame));
    }
    return result;
}

void sese::internal::service::http::HttpConnectionEx::writeSubheaderAndData(const HttpStream::Ptr &stream, const std::string &subheader, size_t remind) {
    auto frame = std::make_unique<sese::net::http::Http2Frame>(max_frame_size);
    frame->ident = stream->id;
    frame->type = sese::net::http::FRAME_TYPE_DATA;
    memcpy(frame->getFrameContentBuffer(), subheader.data(), subheader.length());
    auto l = std::min<size_t>(stream->expect_length - stream->real_length, remind);
    stream->real_length += l;
    stream->file->read(frame->getFrameContentBuffer() + subheader.length(), l);
    frame->length = static_cast<uint32_t>(subheader.length() + l);
    frame->buildFrameHeader();
    pre_vector.push_back(std::move(frame));
}
