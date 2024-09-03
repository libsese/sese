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
    // SESE_DEBUG("Waiting for header");
    readBlock(temp_buffer, 9, [this](const asio::error_code &ec) {
        if (ec) {
            return;
        }
        // SESE_DEBUG("Read Header");
        memset(&frame, 0, sizeof(frame));
        memcpy(reinterpret_cast<char *>(&frame.length) + 1, temp_buffer + 0, 3);
        memcpy(&frame.type, temp_buffer + 3, 1);
        memcpy(&frame.flags, temp_buffer + 4, 1);
        memcpy(&frame.ident, temp_buffer + 5, 4);
        frame.length = FromBigEndian32(frame.length);
        frame.ident = FromBigEndian32(frame.ident);
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
            readFrameHeader();
            // writeAckFrame();
            break;
        }
        case FRAME_TYPE_GOAWAY: {
            handleGoawayFrame();
            break;
        }
        case FRAME_TYPE_HEADERS:
        case FRAME_TYPE_CONTINUATION: {
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
        default:
            break;
    }
}

uint8_t HttpConnectionEx::handleSettingsFrame() {
    SESE_DEBUG("Settings");
    using namespace sese::net::http;
    if (frame.ident != 0) {
        return GOAWAY_PROTOCOL_ERROR;
    }
    if (frame.flags & SETTINGS_FLAGS_ACK) {
        return UINT8_MAX;
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
                break;
            case SETTINGS_INITIAL_WINDOW_SIZE:
                if (*value > 2147483647) {
                    return GOAWAY_FLOW_CONTROL_ERROR;
                }
                this->window_size = *value;
                break;
            default:
                break;
        }
    }
    return 0;
}

void HttpConnectionEx::handleWindowUpdate() {
    SESE_DEBUG("WindowUpdate");
    auto data = reinterpret_cast<uint32_t *>(temp_buffer);
    if (frame.ident == 0) {
        window_size = FromBigEndian32(*data);
    } else {
    }
}

void HttpConnectionEx::handleGoawayFrame() {
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
    SESE_DEBUG("Headers Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    HttpStream::Ptr stream;
    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        stream = std::make_shared<HttpStream>();
        stream->id = frame.ident;
        stream->request.setVersion(HttpVersion::VERSION_2);
        stream->response.setVersion(HttpVersion::VERSION_2);
        streams[frame.ident] = stream;
    } else {
        stream = iterator->second;
    }
    stream->temp_buffer.write(temp_buffer, frame.length);

    if (frame.flags & FRAME_FLAG_END_HEADERS) {
        HPackUtil::decode(&stream->temp_buffer, stream->temp_buffer.getReadableSize(), req_dynamic_table,
                          stream->request);
        stream->temp_buffer.freeCapacity();
        HttpConverter::convertFromHttp2(&stream->request);

        if (frame.flags & FRAME_FLAG_END_STREAM) {
            handleRequest(stream);

            HttpConverter::convert2Http2(&stream->response);
            Header header;
            HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

            writeHeadersFrame(stream);
        }
    }
}

void HttpConnectionEx::handleDataFrame() {
    SESE_DEBUG("Data Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    auto iterator = streams.find(frame.ident);
    auto stream = iterator->second;
    stream->request.getBody().write(temp_buffer, frame.length);
    stream->temp_buffer.freeCapacity();

    if (frame.flags & FRAME_FLAG_END_STREAM) {
        handleRequest(stream);

        HttpConverter::convert2Http2(&stream->response);
        Header header;
        HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);

        writeHeadersFrame(stream);
    }
}

void HttpConnectionEx::handleRstStreamFrame() {
    SESE_DEBUG("Reset Stream Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    uint32_t code;
    memcpy(temp_buffer, &code, 4);
    code = FromBigEndian32(code);
    SESE_WARN("RST STREAM {} CODE {}", frame.ident, code);

    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        writeGoawayFrame(0, frame.ident, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    readFrameHeader();
}


void HttpConnectionEx::handlePriorityFrame() {
    SESE_DEBUG("Priority Frame");
    using namespace sese::net::http;
    if (frame.ident == 0) {
        writeGoawayFrame(0, 0, 0, GOAWAY_PROTOCOL_ERROR, "");
        return;
    }

    HttpStream::Ptr stream;
    auto iterator = streams.find(frame.ident);
    if (iterator == streams.end()) {
        stream = std::make_shared<HttpStream>();
        stream->id = frame.ident;
        stream->request.setVersion(HttpVersion::VERSION_2);
        stream->response.setVersion(HttpVersion::VERSION_2);
        streams[frame.ident] = stream;
    } else {
        stream = iterator->second;
    }
    // 读取负载但不处理
    uint8_t exclusive_flag = 0;
    uint32_t stream_dependency = 0;
    uint8_t weight = 0;

    memcpy(&stream_dependency, temp_buffer, 4);
    stream_dependency = FromBigEndian32(stream_dependency);
    exclusive_flag = (stream_dependency & 0x80000000) >> 31;
    stream_dependency &= 0x7FFFFFFF;
    memcpy(&weight, temp_buffer + 4, 1);

    SESE_DEBUG("Priority set {} deps {}, exclusive {}, weight {}",
               frame.ident,
               stream_dependency,
               exclusive_flag,
               weight);
    readFrameHeader();
}


void HttpConnectionEx::handleRequest(const HttpStream::Ptr &stream) {
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
    }
}

void HttpConnectionEx::writeGoawayFrame(
    uint32_t stream_id,
    uint32_t latest_stream_id,
    uint8_t flags,
    uint32_t error_code,
    const std::string &msg) {
    SESE_DEBUG("Write Goaway");
    using namespace sese::net::http;
    auto frame = std::make_unique<Http2Frame>(msg.length() + 8);
    frame->type = FRAME_TYPE_GOAWAY;
    frame->flags = flags;
    frame->ident = stream_id;
    frame->length = msg.length() + 8;
    frame->buildFrameHeader();
    latest_stream_id = ToBigEndian32(latest_stream_id);
    error_code = ToBigEndian32(error_code);
    memcpy(frame->getFrameContentBuffer() + 0, &latest_stream_id, 4);
    memcpy(frame->getFrameContentBuffer() + 4, &error_code, 4);
    send_queue.push(std::move(frame));
    handleWrite();
}


void HttpConnectionEx::writeSettingsFrame() {
    SESE_DEBUG("Write Settings");
    using namespace sese::net::http;
    std::vector<std::pair<uint16_t, uint32_t> > values = {
        {SETTINGS_INITIAL_WINDOW_SIZE, 1048576},
        {SETTINGS_MAX_FRAME_SIZE, MAX_BUFFER_SIZE},
        {SETTINGS_HEADER_TABLE_SIZE, 8192}
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

    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::writeAckFrame() {
    SESE_DEBUG("Write ACK");
    auto frame = std::make_unique<sese::net::http::Http2Frame>(0);
    frame->type = sese::net::http::FRAME_TYPE_SETTINGS;
    frame->flags = sese::net::http::SETTINGS_FLAGS_ACK;
    frame->buildFrameHeader();
    send_queue.push(std::move(frame));
    handleWrite();
}

void HttpConnectionEx::writeHeadersFrame(const HttpStream::Ptr &stream) {
    auto len = std::min<size_t>(stream->temp_buffer.getReadableSize(), MAX_BUFFER_SIZE);
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
