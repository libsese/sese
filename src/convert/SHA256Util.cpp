#include <sese/convert/SHA256Util.h>
#include <sese/ByteBuilder.h>
#include <sese/Endian.h>
#include <bitset>

#define RightRotate(target, size) ((target >> size) | (target << (32 - size)))

#define h0 hash[0]
#define h1 hash[1]
#define h2 hash[2]
#define h3 hash[3]
#define h4 hash[4]
#define h5 hash[5]
#define h6 hash[6]
#define h7 hash[7]

#define a value[0]
#define b value[1]
#define c value[2]
#define d value[3]
#define e value[4]
#define f value[5]
#define g value[6]
#define h value[7]

using namespace sese;

/// endian 无关的 32 位
struct Bitset32 {
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;

    inline void rightShift3() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   000a'aaaa aaab'bbbb bbbc'cccc cccd'dddd
        byte3 = (byte2 & 0b0000'0111 << 5) | (byte3 & 0b1111'1000 >> 3);
        byte2 = (byte1 & 0b0000'0111 << 5) | (byte2 & 0b1111'1000 >> 3);
        byte1 = (byte0 & 0b0000'0111 << 5) | (byte1 & 0b1111'1000 >> 3);
        byte0 = 0 | (byte1 & 0b1111'1000 >> 3);
    }
    inline void rightShift10() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   0000'0000 00aa'aaaa aabb'bbbb bbcc'cccc
        byte3 = (byte1 & 0b0000'0011 << 6) | (byte2 & 0b1111'1100 >> 2);
        byte2 = (byte0 & 0b0000'0011 << 6) | (byte1 & 0b1111'1100 >> 2);
        byte1 = 0 | (byte0 & 0b1111'1100 >> 2);
        byte0 = 0;
    }

    inline void rightRotate2() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   ddaa'aaaa aabb'bbbb bbcc'cccc ccdd'dddd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0011) << 6 | (bt2 & 0b1111'1100) >> 2
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate6() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   dddd'ddaa aaaa'aabb bbbb'bbcc cccc'ccdd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0011'1111) << 2 | (bt2 & 0b1100'0000) >> 6
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate7() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   dddd'ddda aaaa'aaab bbbb'bbbc cccc'cccd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0111'1111) << 1 | (bt2 & 0b1000'0000) >> 7
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate11() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   cccd'dddd ddda'aaaa aaab'bbbb bbbc'cccc
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0111) << 5 | (bt2 & 0b1111'1000) >> 3
        XX(b0, byte2, byte3);
        XX(b1, byte3, byte0);
        XX(b2, byte0, byte1);
        XX(b3, byte2, byte3);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate13() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   cccc'cddd dddd'daaa aaaa'abbb bbbb'bccc
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0001'1111) << 3 | (bt2 & 0b1110'0000) >> 5
        XX(b0, byte2, byte3);
        XX(b1, byte3, byte0);
        XX(b2, byte0, byte1);
        XX(b3, byte1, byte2);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate17() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bccc'cccc cddd'dddd daaa'aaaa abbb'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0001) << 7 | (bt2 & 0b1111'1110) >> 1
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate18() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbcc'cccc ccdd'dddd ddaa'aaaa aabb'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0011) << 6 | (bt2 & 0b1111'1100) >> 2
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate19() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbbc'cccc cccd'dddd ddda'aaaa aaab'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0111) << 5 | (bt2 & 0b1111'1000) >> 3
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate22() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbbb'bbcc cccc'ccdd dddd'ddaa aaaa'aabb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0011'1111) << 2 | (bt2 & 0b1100'0000) >> 6
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }
    inline void rightRotate25() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   abbb bbbb bccc'cccc cddd'dddd daaa'aaaa
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0001) << 7 | (bt2 & 0b1111'1110) >> 1
        XX(b0, byte0, byte1);
        XX(b1, byte1, byte2);
        XX(b2, byte2, byte3);
        XX(b3, byte3, byte0);
#undef XX
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }

    Bitset32 operator&(const Bitset32 &bit32) const {
        Bitset32 res{};
        res.byte0 = byte0 & bit32.byte0;
        res.byte1 = byte1 & bit32.byte1;
        res.byte2 = byte2 & bit32.byte2;
        res.byte3 = byte3 & bit32.byte3;
        return res;
    }
    Bitset32 operator^(const Bitset32 &bit32) const {
        Bitset32 res{};
        res.byte0 = byte0 ^ bit32.byte0;
        res.byte1 = byte1 ^ bit32.byte1;
        res.byte2 = byte2 ^ bit32.byte2;
        res.byte3 = byte3 ^ bit32.byte3;
        return res;
    }
    Bitset32 operator+(const Bitset32 &bit32) const {
        Bitset32 res{};
        res.byte0 = byte0 + bit32.byte0;
        res.byte1 = byte1 + bit32.byte1;
        res.byte2 = byte2 + bit32.byte2;
        res.byte3 = byte3 + bit32.byte3;
        return res;
    }
    Bitset32 operator~() const {
        Bitset32 res{};
        res.byte0 = ~byte0;
        res.byte1 = ~byte1;
        res.byte2 = ~byte2;
        res.byte3 = ~byte3;
        return res;
    }
};

inline char toChar(unsigned char ch, bool isCap) {
    if (ch >= 0 && ch <= 9) {
        return (char) (ch + 48);
    } else {
        if (isCap) {
            return (char) (ch + 55);
        } else {
            return (char) (ch + 87);
        }
    }
}

const uint32_t SHA256Util::k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

bool SHA256Util::encode(const Stream::Ptr &input, const Stream::Ptr &output) noexcept {
    int64_t size;
    uint32_t buffer[64];
    auto *block = (uint8_t *) buffer;
    // 最终结果
    uint32_t hash[8]{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    while ((size = input->read(block, 64)) == 64) {
        // 构造字
        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }

        // 加密字
        // 中间结果
        uint32_t value[8];
        memcpy(value, hash, sizeof(int32_t) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(hash, value, buffer + i, k[i]);
        }
    }

    // 读取中断
    if (size == -1) return false;

    // 填充再处理一遍
    else if (size <= 55 && size >= 0) {
        memset(block + size + 1, 0, 47 - size);
        block[size] = ToLittleEndian(0x80);
        buffer[15] = ToBigEndian32(size * 8);


        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }

        uint32_t value[8];
        memcpy(value, hash, sizeof(uint32_t) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(hash, value, buffer + i, k[i]);
        }
    }

    // 处理倒数第二次，填充并处理最后一次
    else if (size > 55) {
        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }

        uint32_t value[8];
        memcpy(value, hash, sizeof(uint32_t) * 8);
        for (auto i = 0; i < 64; ++i) {
            // SHA256Util::encode(hash, value, buffer + i, k[i]);
        }

        memset(block, 0, 55);
        block[size] = 0x80;
        buffer[63] = ToBigEndian64(size);

        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }

        memcpy(value, hash, sizeof(uint32_t) * 8);
        for (auto i = 0; i < 64; ++i) {
            // SHA256Util::encode(hash, value, buffer + i, k[i]);
        }
    }

    output->write(hash, 64);
    return true;
}

std::unique_ptr<char[]> SHA256Util::encode(const Stream::Ptr &input, bool isCap) noexcept {
    auto dest = std::make_shared<ByteBuilder>(64);
    auto success = encode(input, dest);
    if (success) {
        unsigned char buffer[32];
        auto rt = std::unique_ptr<char[]>(new char[65]);
        dest->read(buffer, 32);
        for (auto i = 0; i < 32; ++i) {
            rt[i * 2 + 1] = toChar(buffer[i] % 0x10, isCap);
            rt[i * 2 + 0] = toChar(buffer[i] / 0x10, isCap);
        }
        return rt;
    } else {
        return nullptr;
    }
}

void SHA256Util::structure(uint32_t *buffer) noexcept {
    uint32_t *p0 = &buffer[0];
    uint32_t *p1 = &buffer[1];
    uint32_t *p2 = &buffer[9];
    uint32_t *p3 = &buffer[14];
    uint32_t *p4 = &buffer[16];

    uint32_t tmp0 = RightRotate(*p1, 7) ^ RightRotate(*p1, 18) ^ (*p1 >> 3);
    uint32_t tmp1 = RightRotate(*p3, 17) ^ RightRotate(*p3, 19) ^ (*p3 >> 10);

    *p4 = *p0 + tmp0 + *p2 + tmp1;
}

void SHA256Util::encode(uint32_t *hash, uint32_t *value, uint32_t *buffer, uint32_t k) noexcept {
    uint32_t choice = (e & f) ^ ((~e) & g);
    uint32_t majority = (a & b) ^ (a & c) ^ (b & c);
    uint32_t sigma0 = RightRotate(e, 6) ^ RightRotate(e, 11) ^ RightRotate(e, 25);
    uint32_t sigma1 = RightRotate(a, 2) ^ RightRotate(a, 13) ^ RightRotate(a, 22);
    uint32_t tmp1 = h + sigma1 + choice + k + *buffer;
    uint32_t tmp2 = sigma0 + majority;

    h = g;
    g = f;
    f = e;
    e = d + tmp1;
    d = c;
    c = b;
    b = a;
    a = tmp1 + tmp2;

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
}

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h

#undef h0
#undef h1
#undef h2
#undef h3
#undef h4
#undef h5
#undef h6
#undef h7

#undef RightRotate