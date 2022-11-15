#include <sese/convert/SHA256Util.h>
#include <sese/ByteBuilder.h>
#include <sese/Endian.h>
#include <bitset>

using namespace sese;

/// endian 无关的 32 位
struct sese::Bitset32 {
    uint8_t byte0{};
    uint8_t byte1{};
    uint8_t byte2{};
    uint8_t byte3{};

    Bitset32() = default;
    Bitset32(uint32_t value) {
        value = FromBigEndian32(value);
        memcpy(this, &value, sizeof(value));
    }
    Bitset32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
        byte0 = b0;
        byte1 = b1;
        byte2 = b2;
        byte3 = b3;
    }

    void set(uint32_t value) {
        value = ToBigEndian32(value);
        memcpy(this, &value, sizeof(value));
    }

    uint32_t get() {
        return FromBigEndian32(*(uint32_t *) this);
    }

    inline Bitset32 rightShift3() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   000a'aaaa aaab'bbbb bbbc'cccc cccd'dddd
        uint8_t b0 = 0 | (byte0 & 0b1111'1000) >> 3;
        uint8_t b1 = (byte0 & 0b0000'0111) << 5 | (byte1 & 0b1111'1000) >> 3;
        uint8_t b2 = (byte1 & 0b0000'0111) << 5 | (byte2 & 0b1111'1000) >> 3;
        uint8_t b3 = (byte2 & 0b0000'0111) << 5 | (byte3 & 0b1111'1000) >> 3;
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightShift10() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   0000'0000 00aa'aaaa aabb'bbbb bbcc'cccc
        uint8_t b3 = (byte1 & 0b0000'0011) << 6 | (byte2 & 0b1111'1100) >> 2;
        uint8_t b2 = (byte0 & 0b0000'0011) << 6 | (byte1 & 0b1111'1100) >> 2;
        uint8_t b1 = 0 | (byte0 & 0b1111'1100) >> 2;
        uint8_t b0 = 0;
        return {b0, b1, b2, b3};
    }

    inline Bitset32 rightRotate2() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   ddaa'aaaa aabb'bbbb bbcc'cccc ccdd'dddd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0011) << 6 | (bt2 & 0b1111'1100) >> 2
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate6() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   dddd'ddaa aaaa'aabb bbbb'bbcc cccc'ccdd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0011'1111) << 2 | (bt2 & 0b1100'0000) >> 6
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate7() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   dddd'ddda aaaa'aaab bbbb'bbbc cccc'cccd
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0111'1111) << 1 | (bt2 & 0b1000'0000) >> 7
        XX(b0, byte3, byte0);
        XX(b1, byte0, byte1);
        XX(b2, byte1, byte2);
        XX(b3, byte2, byte3);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate11() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   cccd'dddd ddda'aaaa aaab'bbbb bbbc'cccc
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0111) << 5 | (bt2 & 0b1111'1000) >> 3
        XX(b0, byte2, byte3);
        XX(b1, byte3, byte0);
        XX(b2, byte0, byte1);
        XX(b3, byte1, byte2);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate13() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   cccc'cddd dddd'daaa aaaa'abbb bbbb'bccc
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0001'1111) << 3 | (bt2 & 0b1110'0000) >> 5
        XX(b0, byte2, byte3);
        XX(b1, byte3, byte0);
        XX(b2, byte0, byte1);
        XX(b3, byte1, byte2);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate17() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bccc'cccc cddd'dddd daaa'aaaa abbb'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0001) << 7 | (bt2 & 0b1111'1110) >> 1
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate18() const noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbcc'cccc ccdd'dddd ddaa'aaaa aabb'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0011) << 6 | (bt2 & 0b1111'1100) >> 2
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate19() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbbc'cccc cccd'dddd ddda'aaaa aaab'bbbb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0111) << 5 | (bt2 & 0b1111'1000) >> 3
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate22() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   bbbb'bbcc cccc'ccdd dddd'ddaa aaaa'aabb
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0011'1111) << 2 | (bt2 & 0b1100'0000) >> 6
        XX(b0, byte1, byte2);
        XX(b1, byte2, byte3);
        XX(b2, byte3, byte0);
        XX(b3, byte0, byte1);
#undef XX
        return {b0, b1, b2, b3};
    }
    inline Bitset32 rightRotate25() noexcept {
        // from aaaa'aaaa bbbb'bbbb cccc'cccc dddd'dddd
        // to   abbb bbbb bccc'cccc cddd'dddd daaa'aaaa
#define XX(value, bt1, bt2) uint8_t value = (bt1 & 0b0000'0001) << 7 | (bt2 & 0b1111'1110) >> 1
        XX(b0, byte0, byte1);
        XX(b1, byte1, byte2);
        XX(b2, byte2, byte3);
        XX(b3, byte3, byte0);
#undef XX
        return {b0, b1, b2, b3};
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
        uint32_t *p0 = (uint32_t *) this;
        uint32_t *p1 = (uint32_t *) &bit32;
        uint32_t r = ToBigEndian32(FromBigEndian32(*p0) + FromBigEndian32(*p1));
        memcpy(&res, &r, sizeof(uint32_t));
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

const Bitset32 SHA256Util::k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

#define XX(i) value[i] = value[i] + h[i]

bool SHA256Util::encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept {
    int64_t size;
    uint64_t total = 0;
    Bitset32 buffer[64];
    auto *block = (uint8_t *) buffer;

    // 最终结果
    Bitset32 hash[8] {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    // 中间值
    Bitset32 value[8];

    while ((size = input->read(block, 64)) == 64) {
        total += size;
        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }
        memcpy(value, hash, sizeof(Bitset32) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(value, buffer + i, k[i]);
        }
        hash[0] = hash[0] + value[0];
        hash[1] = hash[1] + value[1];
        hash[2] = hash[2] + value[2];
        hash[3] = hash[3] + value[3];
        hash[4] = hash[4] + value[4];
        hash[5] = hash[5] + value[5];
        hash[6] = hash[6] + value[6];
        hash[7] = hash[7] + value[7];
    }

    // 读取中断
    if (size == -1) return false;

    // 填充再处理一遍
    else if (size <= 55 && size >= 0) {
        total += size;
        total *= 8;
        memset(block + size + 1, 0, sizeof(Bitset32) * (47 - size));
        block[size] = ToLittleEndian(0x80);
        total = ToBigEndian64(total * sizeof(uint8_t));
        memcpy(&buffer[14], &total, sizeof(uint64_t));

        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }

        memcpy(value, hash, sizeof(Bitset32) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(value, buffer + i, k[i]);
        }
        hash[0] = hash[0] + value[0];
        hash[1] = hash[1] + value[1];
        hash[2] = hash[2] + value[2];
        hash[3] = hash[3] + value[3];
        hash[4] = hash[4] + value[4];
        hash[5] = hash[5] + value[5];
        hash[6] = hash[6] + value[6];
        hash[7] = hash[7] + value[7];
    }

    // 处理倒数第二次，填充并处理最后一次
    else if (size > 55) {
        block[size] = ToLittleEndian(0x80);
        memset(block + size + 1, 0, 63 - size);
        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }
        memcpy(value, hash, sizeof(Bitset32) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(value, buffer + i, k[i]);
        }
        hash[0] = hash[0] + value[0];
        hash[1] = hash[1] + value[1];
        hash[2] = hash[2] + value[2];
        hash[3] = hash[3] + value[3];
        hash[4] = hash[4] + value[4];
        hash[5] = hash[5] + value[5];
        hash[6] = hash[6] + value[6];
        hash[7] = hash[7] + value[7];

        total += size;
        total *= 8;

        memset(block, 0, 56);
        total = ToBigEndian64(total * sizeof(uint8_t));
        memcpy(&buffer[14], &total, sizeof(uint64_t));

        for (auto i = 0; i < 48; ++i) {
            SHA256Util::structure(buffer + i);
        }
        memcpy(value, hash, sizeof(Bitset32) * 8);
        for (auto i = 0; i < 64; ++i) {
            SHA256Util::encode(value, buffer + i, k[i]);
        }
        hash[0] = hash[0] + value[0];
        hash[1] = hash[1] + value[1];
        hash[2] = hash[2] + value[2];
        hash[3] = hash[3] + value[3];
        hash[4] = hash[4] + value[4];
        hash[5] = hash[5] + value[5];
        hash[6] = hash[6] + value[6];
        hash[7] = hash[7] + value[7];
    }

    output->write(hash, 32);
    return true;
}

#undef XX

std::unique_ptr<char[]> SHA256Util::encode(const InputStream::Ptr &input, bool isCap) noexcept {
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
        rt[64] = 0;
        return rt;
    } else {
        return nullptr;
    }
}

void SHA256Util::structure(Bitset32 *buffer) noexcept {
    Bitset32 *p0 = &buffer[0];
    Bitset32 *p1 = &buffer[1];
    Bitset32 *p2 = &buffer[9];
    Bitset32 *p3 = &buffer[14];
    Bitset32 *p4 = &buffer[16];

    auto tmp0 = p1->rightRotate7() ^ p1->rightRotate18() ^ p1->rightShift3();
    auto tmp1 = p3->rightRotate17() ^ p3->rightRotate19() ^ p3->rightShift10();

    *p4 = *p0 + tmp0 + *p2 + tmp1;
}

#define a value[0]
#define b value[1]
#define c value[2]
#define d value[3]
#define e value[4]
#define f value[5]
#define g value[6]
#define h value[7]

void SHA256Util::encode(Bitset32 *value, Bitset32 *buffer, Bitset32 k) noexcept {
    Bitset32 choice = (e & f) ^ ((~e) & g);
    Bitset32 majority = (a & b) ^ (a & c) ^ (b & c);
    Bitset32 sigma0 = a.rightRotate2() ^ a.rightRotate13() ^ a.rightRotate22();
    Bitset32 sigma1 = e.rightRotate6() ^ e.rightRotate11() ^ e.rightRotate25();
    Bitset32 tmp1 = h + sigma1 + choice + k + *buffer;
    Bitset32 tmp2 = sigma0 + majority;

    h = g;
    g = f;
    f = e;
    e = d + tmp1;
    d = c;
    c = b;
    b = a;
    a = tmp1 + tmp2;
}

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g
#undef h