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
        readFrameHeader();
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
            handleSettingsFrame();
            // writeSettingsFrame();
            writeAckFrame();
            break;
        }
        case FRAME_TYPE_WINDOW_UPDATE: {
            handleWindowUpdate();
            // readFrameHeader();
            writeAckFrame();
            break;
        }
        case FRAME_TYPE_HEADERS:
        case FRAME_TYPE_CONTINUATION: {
            handleHeadersFrame();
            break;
        }
        // case FRAME_TYPE_DATA: {
        //     handleDataFrame();
        //     break;
        // }
        default:
            break;
    }
}

uint8_t HttpConnectionEx::handleSettingsFrame() {
    SESE_DEBUG("Settings");
    using namespace sese::net::http;
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
                if (*value > 16777215) {
                    return GOAWAY_PROTOCOL_ERROR;
                } else {
                    this->max_frame_size = *value;
                }
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
                } else {
                    this->window_size = *value;
                }
                break;
            default:
                // 暂不处理
                break;
        }
    }
    return 0;
}

void HttpConnectionEx::writeSettingsFrame() {
    SESE_DEBUG("Write Settings");
    using namespace sese::net::http;
    std::vector<std::pair<uint16_t, uint32_t> > values = {
        {SETTINGS_INITIAL_WINDOW_SIZE, 1048576},
        {SETTINGS_MAX_FRAME_SIZE, sizeof(temp_buffer)},
        {SETTINGS_HEADER_TABLE_SIZE, 8192}
    };

    frame.length = values.size() * 6;
    frame.type = FRAME_TYPE_SETTINGS;
    frame.flags = 0;
    frame.ident = 0;
    // buildFrameBuffer(nullptr);

    int pos = 9;
    for (auto [key, value]: values) {
        key = ToBigEndian16(key);
        value = ToBigEndian32(value);
        memcpy(temp_buffer + pos, &key, sizeof(key));
        pos += sizeof(key);
        memcpy(temp_buffer + pos, &value, sizeof(value));
        pos += sizeof(value);
    }

    // todo post write
    // writeBlock(temp_buffer, pos, [conn = getPtr()](const asio::error_code &ec) {
    //     if (ec) {
    //         return;
    //     }
    //     conn->readFrameHeader();
    // });
}

void HttpConnectionEx::writeAckFrame() {
    SESE_DEBUG("Write ACK");
    // todo post write
    // uint8_t buffer[]{0x0, 0x0, 0x0, 0x4, 0x1, 0x0, 0x0, 0x0, 0x0};
    // memcpy(temp_buffer, buffer, sizeof(buffer));
    // writeBlock(temp_buffer, sizeof(buffer), [conn = getPtr()](const asio::error_code &ec) {
    //     if (ec) {
    //         return;
    //     }
    //     conn->readFrameHeader();
    // });
}


void HttpConnectionEx::handleWindowUpdate() {
    SESE_DEBUG("WindowUpdate");
    auto data = reinterpret_cast<uint32_t *>(temp_buffer);
    if (frame.ident == 0) {
        window_size = FromBigEndian32(*data);
    } else {
    }
}

void HttpConnectionEx::handleHeadersFrame() {
    using namespace sese::net::http;
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
        }
    }
}

void HttpConnectionEx::handleDataFrame() {
    SESE_INFO("Headers Frame");
    using namespace sese::net::http;
    auto iterator = streams.find(frame.ident);
    auto stream = iterator->second;
    stream->request.getBody().write(temp_buffer, frame.length);
    stream->temp_buffer.freeCapacity();

    if (frame.flags & FRAME_FLAG_END_STREAM) {
        handleRequest(stream);
    }
}

void HttpConnectionEx::handleRequest(const HttpStream::Ptr &stream) {
    auto serv = service.lock();
    assert(serv);
    serv->handleRequest(stream);
    writeHeadersFrame(stream);
}

void HttpConnectionEx::writeHeadersFrame(const HttpStream::Ptr &stream) {
    sese::net::http::Header header;
    sese::net::http::HttpConverter::convert2Http2(&stream->response);
    auto len = sese::net::http::HPackUtil::encode(&stream->temp_buffer, resp_dynamic_table, header, stream->response);
    stream->frame.type = sese::net::http::FRAME_TYPE_HEADERS;
    stream->frame.ident = stream->id;

    if (len <= sizeof(stream->temp_buffer) - 9) {
        stream->frame.flags |= sese::net::http::FRAME_FLAG_END_HEADERS;
    }
    if (stream->conn_type == ConnType::NONE) {
        stream->frame.flags |= sese::net::http::FRAME_FLAG_END_STREAM;
    } else if (stream->conn_type == ConnType::CONTROLLER && stream->response.getBody().getReadableSize() == 0) {
        stream->frame.flags |= sese::net::http::FRAME_FLAG_END_STREAM;
    }
    len = stream->temp_buffer.read(stream->frame_buffer + 9, sizeof(stream->temp_buffer) - 9);
    stream->frame.length = len;

    // todo post write
    // buildFrameBuffer(stream);
    // writeBlock(stream->frame_buffer, 9 + len, [conn = getPtr()](const asio::error_code &code) {
    //     if (code) {
    //         return;
    //     }
    //     conn->readFrameHeader();
    // });
}
