#include "HttpConnectionEx.h"
#include "HttpServiceImpl.h"

#include <sese/io/InputBufferWrapper.h>
#include <sese/util/Endian.h>
#include <sese/net/http/HPackUtil.h>
#include <sese/net/http/HttpConverter.h>
#include <sese/Util.h>
#include <sese/Log.h>

HttpConnectionEx::HttpConnectionEx(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &io_context)
    : timer(io_context),
      service(service) {
}

void HttpConnectionEx::readMagic() {
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

void HttpConnectionEx::readFrameHeader() {
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

        if (frame.length > max_frame_size) {
            return;
        }

        readBlock(temp_buffer, frame.length, [this](const asio::error_code &ec) {
            if (ec) {
                return;
            }
            handleFrameHeader();
        });
    });
}

void HttpConnectionEx::handleFrameHeader() {
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

uint8_t HttpConnectionEx::handleSettingsFrame() {
    // SESE_DEBUG("Settings Frame");
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
    auto input = sese::io::InputBufferWrapper(temp_buffer, frame.length);

    while (input.read(buffer, 6) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case SETTINGS_HEADER_TABLE_SIZE:
                this->req_dynamic_table.resize(*value);
                this->header_table_size = *value;
                break;
            case SETTINGS_MAX_CONCURRENT_STREAMS:
                this->max_concurrent_stream = (*value == 0 ? 100 : *value);
                break;
            case SETTINGS_MAX_FRAME_SIZE:
                if (*value > 16777215 || *value < 16384) {
                    return GOAWAY_PROTOCOL_ERROR;
                }
                this->max_frame_size = *value;
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
                if (is_init_window_size) {
                    return GOAWAY_FLOW_CONTROL_ERROR;
                }
                is_init_window_size = true;
                if (*value > 2147483647) {
                    return GOAWAY_FLOW_CONTROL_ERROR;
                }
                this->init_window_size = *value;
                break;
            default:
                break;
        }
    }
    return 0;
}

void HttpConnectionEx::handleWindowUpdate() {
    // SESE_DEBUG("WindowUpdate Frame");
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
        if (sese::isAdditionOverflow<int32_t>(static_cast<int32_t>(init_window_size), static_cast<int32_t>(i))) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_FLOW_CONTROL_ERROR, "");
            return;
        }
        for (auto &[ident, stream]: streams) {
            stream->write_window_size += i;
        }
    } else {
        auto iterator = streams.find(frame.ident);
        if (iterator == streams.end()) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }
        iterator->second->continue_type = frame.type;
        if (sese::isAdditionOverflow<int32_t>(static_cast<int32_t>(iterator->second->write_window_size),
                                              static_cast<int32_t>(i))) {
            writeRstStreamFrame(frame.ident, 0, GOAWAY_FLOW_CONTROL_ERROR);
            return;
        }
        iterator->second->write_window_size += i;
    }

    readFrameHeader();
}

void HttpConnectionEx::handleGoawayFrame() {
    // SESE_DEBUG("Goaway Frame");
    using namespace sese::net::http;
    if (frame.ident != 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    iterator->second->continue_type = frame.type;

    uint32_t latest_stream;
    memcpy(&latest_stream, temp_buffer, sizeof(latest_stream));
    latest_stream = FromBigEndian32(latest_stream);
    uint32_t error_code;
    memcpy(&error_code, temp_buffer + 4, sizeof(latest_stream));
    error_code = FromBigEndian32(error_code);
    if (frame.length - 8) {
        auto msg = std::string(temp_buffer + 8, frame.length - 8);
        SESE_WARN("FAILED: F:0x{:x} S:{} LS:{} CODE:{} MSG:{}",
                  frame.flags,
                  frame.ident,
                  latest_stream,
                  error_code,
                  msg);
    } else {
        SESE_WARN("FAILED: F:0x{:x} S:{} LS:{} CODE:{}",
                  frame.flags,
                  frame.ident,
                  latest_stream,
                  error_code);
    }
    readFrameHeader();
}

void HttpConnectionEx::handleHeadersFrame() {
    // SESE_DEBUG("Headers Frame");
    using namespace sese::net::http;
    if (expect_ack) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "expect ack");
        return;
    }

    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    HttpStream::Ptr stream;
    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        stream = std::make_shared<HttpStream>();
        stream->id = frame.ident;
        stream->write_window_size = init_window_size;
        stream->request.setVersion(HttpVersion::VERSION_2);
        stream->response.setVersion(HttpVersion::VERSION_2);
        streams[frame.ident] = stream;
    } else {
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
        auto rt = HPackUtil::decode(&stream->temp_buffer, stream->temp_buffer.getReadableSize(), req_dynamic_table,
                                    stream->request);
        stream->temp_buffer.freeCapacity();
        if (!rt) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_COMPRESSION_ERROR, "");
            return;
        }

        rt = HttpConverter::convertFromHttp2(&stream->request);
        if (!rt) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }

        if (stream->end_stream) {
            handleRequest(stream);

            HttpConverter::convert2Http2(&stream->response);
            Header header;
            HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

            writeHeadersFrame(stream);
        } else {
            readFrameHeader();
        }
    } else {
        readFrameHeader();
    }
}

void HttpConnectionEx::handleDataFrame() {
    // SESE_DEBUG("Data Frame");
    using namespace sese::net::http;
    if (expect_ack) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "expect ack");
        return;
    }

    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    auto stream = iterator->second;
    stream->continue_type = frame.type;

    if (frame.flags & FRAME_FLAG_PADDED) {
        uint8_t padded = temp_buffer[0];
        if (padded > frame.length) {
            writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
            return;
        }

        stream->request.getBody().write(temp_buffer + 1, frame.length - padded - 1);
    } else {
        stream->request.getBody().write(temp_buffer, frame.length);
    }

    stream->temp_buffer.freeCapacity();

    if (frame.flags & FRAME_FLAG_END_STREAM) {
        if (stream->request.exist("content-length")) {
            auto content_length = sese::toInteger(stream->request.get("content-length"));
            if (content_length != stream->request.getBody().getReadableSize()) {
                writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
                return;
            }
        }

        handleRequest(stream);

        HttpConverter::convert2Http2(&stream->response);
        Header header;
        HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

        writeHeadersFrame(stream);
    } else {
        readFrameHeader();
    }
}

void HttpConnectionEx::handleRstStreamFrame() {
    // SESE_DEBUG("Reset Stream Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    if (frame.length != 4) {
        writeGoawayFrame(0, 0, GOAWAY_FRAME_SIZE_ERROR, "");
        return;
    }

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }
    iterator->second->continue_type = frame.type;

    uint32_t code;
    memcpy(temp_buffer, &code, 4);
    code = FromBigEndian32(code);
    // SESE_WARN("RST STREAM {} CODE {}", frame.ident, code);

    readFrameHeader();
}


void HttpConnectionEx::handlePriorityFrame() {
    // SESE_DEBUG("Priority Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
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
        stream = std::make_shared<HttpStream>();
        stream->id = frame.ident;
        stream->write_window_size = init_window_size;
        stream->request.setVersion(HttpVersion::VERSION_2);
        stream->response.setVersion(HttpVersion::VERSION_2);
        streams[frame.ident] = stream;
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
    // SESE_DEBUG("Priority set {} deps {}, exclusive {}, weight {}",
    //            frame.ident,
    //            stream_dependency,
    //            exclusive_flag,
    //            weight);
    readFrameHeader();
}

void HttpConnectionEx::handlePingFrame() {
    // SESE_DEBUG("Headers Frame");
    using namespace sese::net::http;
    if (frame.ident != 0) {
        // writeGoawayFrame(0, 0, GOAWAY_PROTOCOL_ERROR, "");
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

    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::handleRequest(const HttpStream::Ptr &stream) {
    // NOLINT
    auto serv = service.lock();
    assert(serv);
    serv->handleRequest(stream);
}

void HttpConnectionEx::handleWrite() {
    if (!is_read) {
        this->readFrameHeader();
    }

    if (!send_queue.empty()) {
        auto &frame = send_queue.front();
        writeBlock(frame->getFrameBuffer(), frame->getFrameLength(), [conn = getPtr()](const asio::error_code &ec) {
            if (ec) {
                return;
            }
            conn->send_queue.pop();
            conn->handleWrite();
        });
    } else {
        // SESE_WARN("Empty");
    }
}

void HttpConnectionEx::writeGoawayFrame(
    uint32_t latest_stream_id,
    uint8_t flags,
    uint32_t error_code,
    const std::string &msg) {
    // SESE_DEBUG("Write Goaway");
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
    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::writeRstStreamFrame(
    uint32_t stream_id,
    uint8_t flags,
    uint32_t error_code) {
    // SESE_DEBUG("Write RstStream");
    using namespace sese::net::http;
    auto frame = std::make_unique<Http2Frame>(4);
    frame->type = FRAME_TYPE_RST_STREAM;
    frame->flags = flags;
    frame->ident = stream_id;
    frame->length = 4;
    frame->buildFrameHeader();
    error_code = ToBigEndian32(error_code);
    memcpy(frame->getFrameContentBuffer(), &error_code, 4);
    send_queue.push(std::move(frame));
    handleWrite();
}


void HttpConnectionEx::writeSettingsFrame() {
    // SESE_DEBUG("Write Settings");
    using namespace sese::net::http;
    std::vector<std::pair<uint16_t, uint32_t> > values = {
        {SETTINGS_INITIAL_WINDOW_SIZE, INIT_WINDOW_SIZE},
        {SETTINGS_MAX_FRAME_SIZE, MAX_FRAME_SIZE},
        {SETTINGS_HEADER_TABLE_SIZE, HEADER_TABLE_SIZE}
    };

    auto frame = std::make_unique<sese::net::http::Http2Frame>(values.size() * 6);
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
    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::writeAckFrame() {
    // SESE_DEBUG("Write ACK");
    auto frame = std::make_unique<sese::net::http::Http2Frame>(0);
    frame->type = sese::net::http::FRAME_TYPE_SETTINGS;
    frame->flags = sese::net::http::SETTINGS_FLAGS_ACK;
    frame->buildFrameHeader();
    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::writeHeadersFrame(const HttpStream::Ptr &stream) {
    auto len = std::min<size_t>(stream->temp_buffer.getReadableSize(), MAX_FRAME_SIZE);
    auto frame = std::make_unique<sese::net::http::Http2Frame>(len);
    stream->temp_buffer.read(frame->getFrameContentBuffer(), len);
    frame->ident = stream->id;
    frame->type = sese::net::http::FRAME_TYPE_HEADERS;
    frame->length = static_cast<uint32_t>(len);

    if (stream->temp_buffer.getReadableSize() == 0) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_HEADERS;
    }
    if (stream->response.getBody().getReadableSize() == 0) {
        frame->flags |= sese::net::http::FRAME_FLAG_END_STREAM;
    }
    frame->buildFrameHeader();
    send_queue.push(std::move(frame));
    handleWrite();
}
