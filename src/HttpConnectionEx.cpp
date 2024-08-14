#include "HttpConnectionEx.h"
#include "HttpServiceImpl.h"

#include <sese/io/InputBufferWrapper.h>
#include <sese/util/Endian.h>
#include <sese/net/http/HPackUtil.h>
#include <sese/net/http/HttpConverter.h>
#include <sese/Util.h>

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
            break;
        }
        case FRAME_TYPE_WINDOW_UPDATE: {
            handleWindowUpdate();
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
        default:
            break;
    }
    readFrameHeader();
}

uint8_t HttpConnectionEx::handleSettingsFrame() {
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

void HttpConnectionEx::handleWindowUpdate() {
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
    // todo do response
}
