#include "sese/convert/PercentConverter.h"

using sese::OutputStream;
using sese::PercentConverter;

char inline getEncodeChar(unsigned char ch) {
    return ch + (ch >= 0 && ch <= 9 ? 48 : 55);
}

void PercentConverter::encode(const char *src, const OutputStream::Ptr &dest) {
    unsigned char buffer[3]{'%'};
    auto *p = reinterpret_cast<const unsigned char *>(src);
    while (*p != 0) {
        buffer[1] = getEncodeChar(*p / 0x10);
        buffer[2] = getEncodeChar(*p % 0x10);
        dest->write(buffer, 3);
        p++;
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
            return false;
        }
    }
    return true;
}