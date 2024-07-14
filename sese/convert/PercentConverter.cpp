#include "sese/convert/PercentConverter.h"
#include "sese/text/StringBuilder.h"

using sese::PercentConverter;
using sese::io::OutputStream;

const std::set<char> PercentConverter::URL_EXCLUDE_CHARS{
        '!', '*', '\"', '\'', '(', ')', ';', ':', '@', '&',
        '=', '+', '$', ',', '/', '?', '%', '#', '[', ']', ' '};

char inline getEncodeChar(unsigned char ch) {
    return ch + (ch >= 0 && ch <= 9 ? 48 : 55);
}

void PercentConverter::encode(const char *src, OutputStream *dest) {
    unsigned char buffer[3]{'%', 0, 0};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            if (URL_EXCLUDE_CHARS.count(static_cast<char>(*p))) {
                buffer[1] = getEncodeChar(*p / 0x10);
                buffer[2] = getEncodeChar(*p % 0x10);
                dest->write(buffer, 3);
                p++;
            } else {
                dest->write(p, 1);
                p++;
            }
        } else {
            buffer[1] = getEncodeChar(*p / 0x10);
            buffer[2] = getEncodeChar(*p % 0x10);
            dest->write(buffer, 3);
            p++;
        }
    }
}

void PercentConverter::encode(const char *src, const OutputStream::Ptr &dest) {
    encode(src, dest.get());
}

char inline getHexChar(char ch) {
    // GCOVR_EXCL_START
    if (ch >= 'A' && ch <= 'F') {
        return ch - 55;
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 87;
    }
    if (ch >= '0' && ch <= '9') {
        return ch - 48;
    }
    // GCOVR_EXCL_STOP
    return static_cast<char>(-1);
}

bool PercentConverter::decode(const char *src, const OutputStream::Ptr &dest) {
    return decode(src, dest.get());
}

bool PercentConverter::decode(const char *src, OutputStream *dest) {
    char decode_char;
    char ch1;
    char ch2;

    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (*p == '%') {
            ch1 = getHexChar(p[1]);
            if (ch1 == static_cast<char>(-1)) {
                return false;
            }
            ch2 = getHexChar(p[2]);
            if (ch2 == static_cast<char>(-1)) {
                return false;
            }
            decode_char = ch1 * 0x10 + ch2;
            dest->write((const void *) &decode_char, 1);
            p += 3;
        } else {
            dest->write(p, 1);
            p += 1;
        }
    }
    return true;
}

std::string sese::PercentConverter::encode(const char *src) {
    text::StringBuilder builder;
    unsigned char buffer[4]{'%', 0, 0, 0};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            if (URL_EXCLUDE_CHARS.count(static_cast<char>(*p))) {
                buffer[1] = getEncodeChar(*p / 0x10);
                buffer[2] = getEncodeChar(*p % 0x10);
                builder.append(reinterpret_cast<const char *>(buffer));
                p++;
            } else {
                builder.append(static_cast<char>(*p));
                p++;
            }
        } else {
            buffer[1] = getEncodeChar(*p / 0x10);
            buffer[2] = getEncodeChar(*p % 0x10);
            builder.append(reinterpret_cast<const char *>(buffer));
            p++;
        }
    }
    return builder.toString();
}

std::string sese::PercentConverter::decode(const char *src) {
    text::StringBuilder builder;
    char decode_char;
    char ch1;
    char ch2;

    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (*p == '%') {
            ch1 = getHexChar(p[1]);
            if (ch1 == static_cast<char>(-1)) {
                return {};
            }
            ch2 = getHexChar(p[2]);
            if (ch2 == static_cast<char>(-1)) {
                return {};
            }
            decode_char = ch1 * 0x10 + ch2;
            builder.append(decode_char);
            p += 3;
        } else {
            builder.append(static_cast<char>(*p));
            p += 1;
            continue;
        }
    }
    return builder.toString();
}
