#include "sese/convert/PercentConverter.h"
#include "sese/text/StringBuilder.h"

using sese::OutputStream;
using sese::PercentConverter;

std::set<char> sese::PercentConverter::urlExcludeChars{
        '!', '*', '\"', '\'', '(', ')', ';', ':', '@', '&',
        '=', '+', '$', ',', '/', '?', '%', '#', '[', ']', ' '};

char inline getEncodeChar(unsigned char ch) {
    return ch + (ch >= 0 && ch <= 9 ? 48 : 55);
}

void PercentConverter::encode(const char *src, sese::OutputStream *dest) {
    unsigned char buffer[3]{'%'};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            dest->write(p, 1);
            p++;
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

void PercentConverter::encode(const char *src, const OutputStream::Ptr &dest, std::set<char> &excludeChars) {
    encode(src, dest.get(), excludeChars);
}

void PercentConverter::encode(const char *src, sese::OutputStream *dest, std::set<char> &excludeChars) {
    unsigned char buffer[3]{'%'};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            if (excludeChars.count((char) *p)) {
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

char inline getHexChar(char ch) {
    if (ch >= 'A' && ch <= 'F') {
        return ch - 55;
    } else if (ch >= 'a' && ch <= 'f') {
        return ch - 87;
    } else if (ch >= '0' && ch <= '9') {
        return ch - 48;
    } else {
        return -1;
    }
}

bool PercentConverter::decode(const char *src, const OutputStream::Ptr &dest) {
    return decode(src, dest.get());
}

bool PercentConverter::decode(const char *src, sese::OutputStream *dest) {
    char decodeChar;
    char ch1;
    char ch2;

    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (*p == '%') {
            ch1 = getHexChar(p[1]);
            if (ch1 == -1) {
                return false;
            }
            ch2 = getHexChar(p[2]);
            if (ch2 == -1) {
                return false;
            }
            decodeChar = ch1 * 0x10 + ch2;
            dest->write((const void *) &decodeChar, 1);
            p += 3;
        } else {
            dest->write(p, 1);
            p += 1;
        }
    }
    return true;
}

std::string PercentConverter::encode(const char *src) {
    text::StringBuilder builder;
    unsigned char buffer[4]{'%', 0, 0, 0};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            builder.append((char) *p);
            p++;
        } else {
            buffer[1] = getEncodeChar(*p / 0x10);
            buffer[2] = getEncodeChar(*p % 0x10);
            builder.append((const char *) buffer);
            p++;
        }
    }
    return builder.toString();
}
std::string sese::PercentConverter::encode(const char *src, std::set<char> &excludeChars) {
    text::StringBuilder builder;
    unsigned char buffer[4]{'%', 0, 0, 0};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (((*p & 0b1000'0000) >> 7) == 0) {
            if (excludeChars.count((char) *p)) {
                buffer[1] = getEncodeChar(*p / 0x10);
                buffer[2] = getEncodeChar(*p % 0x10);
                builder.append((const char *) buffer);
                p++;
            } else {
                builder.append((char) *p);
                p++;
            }
        } else {
            buffer[1] = getEncodeChar(*p / 0x10);
            buffer[2] = getEncodeChar(*p % 0x10);
            builder.append((const char *) buffer);
            p++;
        }
    }
    return builder.toString();
}

std::string sese::PercentConverter::decode(const char *src) {
    text::StringBuilder builder;
    char decodeChar;
    char ch1;
    char ch2;

    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        if (*p == '%') {
            ch1 = getHexChar(p[1]);
            if (ch1 == -1) {
                return {};
            }
            ch2 = getHexChar(p[2]);
            if (ch2 == -1) {
                return {};
            }
            decodeChar = ch1 * 0x10 + ch2;
            builder.append(decodeChar);
            p += 3;
        } else {
            builder.append((char) *p);
            p += 1;
            continue;
        }
    }
    return builder.toString();
}
