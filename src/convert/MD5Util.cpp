#include "sese/convert/MD5Util.h"
#include "sese/ByteBuilder.h"
#include <cmath>
#include <cstring>

using sese::ByteBuilder;
using sese::MD5Util;

bool MD5Util::encode(const sese::Stream::Ptr &input, const sese::Stream::Ptr &output) noexcept {
    uint32_t result[4]{A, B, C, D};
    unsigned char buffer[64];
    uint64_t length = 0;
    while (true) {
        int64_t len = input->read(buffer, 64);
        if (len == 64) {
            length += len;
        } else if (len > 0 && len <= 56) {
            length += len;
            memcpy(&buffer[len], PADDING, 64 - len - 8);
            length *= 8;/// 单位为 位
            length = ToLittleEndian64(length);
            memcpy(&buffer[56], &length, 8);
        } else if (len > 56) {
            length += len;
            memset(&buffer[len], 0, 64 - len);
            input->write(PADDING, 56);
        } else if (len == 0) {
            break;
        } else {
            return false;
        }

        auto *x = (uint32_t *) &buffer;
        uint32_t a = result[0];
        uint32_t b = result[1];
        uint32_t c = result[2];
        uint32_t d = result[3];

        //第1轮循环变换
        a = FF(a, b, c, d, x[0], S11, 0xd76aa478);  /* 1 */
        d = FF(d, a, b, c, x[1], S12, 0xe8c7b756);  /* 2 */
        c = FF(c, d, a, b, x[2], S13, 0x242070db);  /* 3 */
        b = FF(b, c, d, a, x[3], S14, 0xc1bdceee);  /* 4 */
        a = FF(a, b, c, d, x[4], S11, 0xf57c0faf);  /* 5 */
        d = FF(d, a, b, c, x[5], S12, 0x4787c62a);  /* 6 */
        c = FF(c, d, a, b, x[6], S13, 0xa8304613);  /* 7 */
        b = FF(b, c, d, a, x[7], S14, 0xfd469501);  /* 8 */
        a = FF(a, b, c, d, x[8], S11, 0x698098d8);  /* 9 */
        d = FF(d, a, b, c, x[9], S12, 0x8b44f7af);  /* 10 */
        c = FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
        b = FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
        a = FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
        d = FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
        c = FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
        b = FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

        //第2轮循环变换
        a = GG(a, b, c, d, x[1], S21, 0xf61e2562);  /* 17 */
        d = GG(d, a, b, c, x[6], S22, 0xc040b340);  /* 18 */
        c = GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
        b = GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);  /* 20 */
        a = GG(a, b, c, d, x[5], S21, 0xd62f105d);  /* 21 */
        d = GG(d, a, b, c, x[10], S22, 0x2441453);  /* 22 */
        c = GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
        b = GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);  /* 24 */
        a = GG(a, b, c, d, x[9], S21, 0x21e1cde6);  /* 25 */
        d = GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
        c = GG(c, d, a, b, x[3], S23, 0xf4d50d87);  /* 27 */
        b = GG(b, c, d, a, x[8], S24, 0x455a14ed);  /* 28 */
        a = GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
        d = GG(d, a, b, c, x[2], S22, 0xfcefa3f8);  /* 30 */
        c = GG(c, d, a, b, x[7], S23, 0x676f02d9);  /* 31 */
        b = GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
        //第3轮循环变换
        a = HH(a, b, c, d, x[5], S31, 0xfffa3942);  /* 33 */
        d = HH(d, a, b, c, x[8], S32, 0x8771f681);  /* 34 */
        c = HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
        b = HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
        a = HH(a, b, c, d, x[1], S31, 0xa4beea44);  /* 37 */
        d = HH(d, a, b, c, x[4], S32, 0x4bdecfa9);  /* 38 */
        c = HH(c, d, a, b, x[7], S33, 0xf6bb4b60);  /* 39 */
        b = HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
        a = HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
        d = HH(d, a, b, c, x[0], S32, 0xeaa127fa);  /* 42 */
        c = HH(c, d, a, b, x[3], S33, 0xd4ef3085);  /* 43 */
        b = HH(b, c, d, a, x[6], S34, 0x4881d05);   /* 44 */
        a = HH(a, b, c, d, x[9], S31, 0xd9d4d039);  /* 45 */
        d = HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
        c = HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
        b = HH(b, c, d, a, x[2], S34, 0xc4ac5665);  /* 48 */
        //第4轮循环变换
        a = II(a, b, c, d, x[0], S41, 0xf4292244);  /* 49 */
        d = II(d, a, b, c, x[7], S42, 0x432aff97);  /* 50 */
        c = II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
        b = II(b, c, d, a, x[5], S44, 0xfc93a039);  /* 52 */
        a = II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
        d = II(d, a, b, c, x[3], S42, 0x8f0ccc92);  /* 54 */
        c = II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
        b = II(b, c, d, a, x[1], S44, 0x85845dd1);  /* 56 */
        a = II(a, b, c, d, x[8], S41, 0x6fa87e4f);  /* 57 */
        d = II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
        c = II(c, d, a, b, x[6], S43, 0xa3014314);  /* 59 */
        b = II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
        a = II(a, b, c, d, x[4], S41, 0xf7537e82);  /* 61 */
        d = II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
        c = II(c, d, a, b, x[2], S43, 0x2ad7d2bb);  /* 63 */
        b = II(b, c, d, a, x[9], S44, 0xeb86d391);  /* 64 */

        result[0] += a;
        result[1] += b;
        result[2] += c;
        result[3] += d;
    }

    output->write(result, 16);
    return true;
}

inline char toChar(unsigned char ch, bool isCap) {
    if (ch >= 0 && ch <= 9) {
        return (char) (ch + 48);
    } else {
        if(isCap) {
            return (char) (ch + 55);
        }else{
            return (char) (ch + 87);
        }
    }
}

std::string MD5Util::encode(const Stream::Ptr &input, bool isCap) noexcept {
    auto dest = std::make_shared<ByteBuilder>(16);
    auto success = encode(input, dest);
    if (success) {
        unsigned char buffer[16];
        char md5String[33]{0};
        dest->read(buffer, 16);
        for (size_t i = 0; i < 16; i++) {
            md5String[i * 2 + 1] = toChar(buffer[i] % 0x10, isCap);
            md5String[i * 2 + 0] = toChar(buffer[i] / 0x10, isCap);
        }
        return {md5String};
    } else {
        return {};
    }
}

uint32_t MD5Util::F(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return (x & y) | ((~x) & z);
}

uint32_t MD5Util::G(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return (x & z) | (y & (~z));
}

uint32_t MD5Util::H(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return x ^ y ^ z;
}

uint32_t MD5Util::I(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return y ^ (x | (~z));
}

#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

uint32_t MD5Util::FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept {
    a += F(b, c, d) + x + t;
    a = ROL(a, s);
    a += b;
    return a;
}

uint32_t MD5Util::GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept {
    a += G(b, c, d) + x + t;
    a = ROL(a, s);
    a += b;
    return a;
}

uint32_t MD5Util::HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept {
    a += H(b, c, d) + x + t;
    a = ROL(a, s);
    a += b;
    return a;
}

uint32_t MD5Util::II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept {
    a += I(b, c, d) + x + t;
    a = ROL(a, s);
    a += b;
    return a;
}

#undef ROL
