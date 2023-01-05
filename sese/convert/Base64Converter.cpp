#include "sese/convert/Base64Converter.h"

using sese::Base64Converter;
using sese::Stream;

/// 标准 BASE64 码表
constexpr static const unsigned char Base64CodePage[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void inline encode(unsigned char in, unsigned char &out, Base64Converter::CodePage codePage) {
    if (in < 62) {
        out = Base64CodePage[in];
    } else {
        if (codePage == Base64Converter::CodePage::BASE62) {
            if (in == 62) out = '-';
            else
                out = '_';
        } else {
            if (in == 62) out = '+';
            else
                out = '/';
        }
    }
}

void Base64Converter::encode(InputStream *src, OutputStream *dest, CodePage codePage) {
    unsigned char buffer[4]{0};

    int64_t len;
    while ((len = src->read(buffer, 3)) != 0) {
        if (len == 2) {
            buffer[2] = 0b00000000;
        } else if (len == 1) {
            buffer[1] = 0b00000000;
            buffer[2] = 0b00000000;
        }

        unsigned char bits = 0b00000000;
        unsigned char preBits = 0b00000000;
        // 1
        bits = (buffer[0] & 0b11111100) >> 2;
        preBits = buffer[0];
        ::encode(bits, buffer[0], codePage);
        // 2
        bits = (preBits & 0b00000011) << 4 | (buffer[1] & 0b11110000) >> 4;
        preBits = buffer[1];
        ::encode(bits, buffer[1], codePage);
        // 3
        if(len >= 2) {
            bits = (preBits & 0b00001111) << 2 | (buffer[2] & 0b11000000) >> 6;
            preBits = buffer[2];
            ::encode(bits, buffer[2], codePage);

            // 4
            if (len == 3 ) {
                bits = (preBits & 0b00111111);
                ::encode(bits, buffer[3], codePage);
            }
        }
        dest->write(buffer, len + 1);
        if (len != 3) dest->write("==", len == 1 ? 2 : 1);
    }
}

void Base64Converter::encode(const InputStream::Ptr &src, const OutputStream::Ptr &dest, CodePage codePage) {
    encode(src.get(), dest.get(), codePage);
}

void Base64Converter::decode(InputStream *src, OutputStream *dest) {
    unsigned char buffer[4]{0};

    int64_t len;
    while ((len = src->read(buffer, 4)) != 0) {
        for(auto i = 0; i < len; i++){
            if(buffer[i] >= 'A' && buffer[i] <= 'Z') {
                buffer[i] -= 65;
            } else if(buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 71;
            } else if(buffer[i] >= '0' && buffer[i] <= '9') {
                buffer[i] += 4;
            } else if(buffer[i] == '+' || buffer[i] == '-') {
                buffer[i] = 62;
            } else if(buffer[i] == '/' || buffer[i] == '_') {
                buffer[i] = 63;
            } else {
                buffer[i] = 0;
            }
        }

        // 1
        if(len >= 2) {
            buffer[0] = (buffer[0] << 2) | (buffer[1] & 0b00110000) >> 4;

            // 2
            if(len >= 3) {
                buffer[1] = (buffer[1] & 0b00001111) << 4 | (buffer[2] & 0b00111100) >> 2;

                // 3
                if(len == 4){
                    buffer[2] = (buffer[2] & 0b00000011) << 6 | buffer[3];
                }
            }
        }

        dest->write(buffer, len - 1);
    }
}

void Base64Converter::decode(const InputStream::Ptr &src, const OutputStream::Ptr &dest) {
    decode(src.get(), dest.get());
}