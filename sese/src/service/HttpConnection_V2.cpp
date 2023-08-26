#include <sese/service/HttpConnection_V2.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/convert/Base64Converter.h>
#include <sese/net/http/Http2FrameInfo.h>

bool sese::service::v2::HttpConnectionWrapper::readFrame(net::http::Http2FrameInfo &frame) noexcept {
    uint8_t buffer[9]{};
    if (buffer2read.read(buffer, sizeof(buffer)) != sizeof(buffer)) {
        return false;
    }

    memset(&frame, 0, sizeof(frame));
    memcpy(((char *) &frame.length) + 1, buffer + 0, 3);
    memcpy(&frame.type, buffer + 3, 1);
    memcpy(&frame.flags, buffer + 4, 1);
    memcpy(&frame.ident, buffer + 5, 4);

    frame.length = FromBigEndian32(frame.length);
    frame.ident = FromBigEndian32(frame.ident);
    return true;
}

void sese::service::v2::HttpConnectionWrapper::writeFrame(const net::http::Http2FrameInfo &frame) noexcept {
    auto len = ToBigEndian32(frame.length);
    auto ident = ToBigEndian32(frame.ident);

    char buffer[9];
    memcpy(buffer + 0, ((const char *) &len) + 1, 3);
    memcpy(buffer + 3, &frame.type, 1);
    memcpy(buffer + 4, &frame.flags, 1);
    memcpy(buffer + 5, &ident, 4);

    buffer2write.write(buffer, 9);
}

void sese::service::v2::HttpConnectionWrapper::writeAck() noexcept {
    uint8_t buffer[]{0x0, 0x0, 0x0, 0x4, 0x1, 0x0, 0x0, 0x0, 0x0};
    buffer2write.write(buffer, 9);
}

void sese::service::v2::HttpConnectionWrapper::writeRST(uint32_t id) noexcept {
#pragma pack(push, 1)
    struct Buffer {
        uint8_t _0 = 0;
        uint8_t _1 = 0;
        uint8_t _2 = 0;
        uint8_t _3 = 3;
        uint8_t _4 = 0;
        uint32_t _id = 0;
    } buffer;
    buffer._id = ToBigEndian32(id);
    buffer2write.write(&buffer, sizeof(buffer));
#pragma pack(pop)
}

int sese::service::v2::Http2Connection::decodeHttp2Settings(const std::string &settings) noexcept {
    char buffer[6];
    auto ident = (uint16_t *) &buffer[0];
    auto value = (uint32_t *) &buffer[2];
    auto input = sese::InputBufferWrapper(settings.c_str(), settings.length());
    auto output = sese::ByteBuilder(1024);
    sese::Base64Converter::decode(&input, &output);

    while (output.read(buffer, 6) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case net::http::SETTINGS_HEADER_TABLE_SIZE:
                this->dynamicTable1.resize(*value);
                this->headerTableSize = *value;
                break;
            case net::http::SETTINGS_MAX_CONCURRENT_STREAMS:
                this->maxConcurrentStream = (*value == 0 ? 100 : *value);
                break;
            case net::http::SETTINGS_MAX_FRAME_SIZE:
                if (*value > 16777215) {
                    // sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                    // conn->context.close();
                    return net::http::GOAWAY_PROTOCOL_ERROR;
                } else {
                    this->maxFrameSize = *value;
                }
                break;
            case net::http::SETTINGS_ENABLE_PUSH:
                if (*value <= 1) {
                    this->enablePush = *value;
                } else {
                    // sendGoaway(conn, 0, GOAWAY_PROTOCOL_ERROR);
                    // conn->context.close();
                    return net::http::GOAWAY_PROTOCOL_ERROR;
                }
                break;
            case net::http::SETTINGS_MAX_HEADER_LIST_SIZE:
                this->maxHeaderListSize = *value;
                break;
            case net::http::SETTINGS_INITIAL_WINDOW_SIZE:
                if (*value > 2147483647) {
                    // sendGoaway(conn, 0, GOAWAY_FLOW_CONTROL_ERROR);
                    // conn->context.close();
                    return net::http::GOAWAY_FLOW_CONTROL_ERROR;
                } else {
                    this->windowSize = *value;
                }
                break;
            default:
                // 暂不处理
                break;
        }
    }
    return 0;
}