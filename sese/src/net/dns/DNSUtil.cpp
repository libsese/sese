#include <sese/net/dns/DNSUtil.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Endian.h>
#include <sese/util/InputBufferWrapper.h>

void sese::net::dns::DNSUtil::decodeFrameHeaderInfo(const uint8_t buf[12], sese::net::dns::FrameHeaderInfo &info) noexcept {
    info.transactionId = buf[0] * 0x100 + buf[1];
    decodeFrameFlagsInfo(&buf[2], info.flags);
    info.questions = buf[4] * 0x100 + buf[5];
    info.answerPrs = buf[6] * 0x100 + buf[7];
    info.authorityPrs = buf[8] * 0x100 + buf[9];
    info.additionalPrs = buf[10] * 0x100 + buf[11];
}

void sese::net::dns::DNSUtil::decodeFrameFlagsInfo(const uint8_t buf[2], sese::net::dns::FrameFlagsInfo &info) noexcept {
    info.QR = (buf[0] & 0b1000'0000) >> 7;
    info.opcode = (buf[0] & 0b0111'1000) >> 3;
    info.AA = (buf[0] & 0b0000'0100) >> 2;
    info.TC = (buf[0] & 0b0000'0010) >> 1;
    info.RD = (buf[0] & 0b0000'0001);
    info.RA = (buf[1] & 0b1000'0000) >> 7;
    info.z = (buf[1] & 0b0111'0000) >> 4;
    info.rcode = (buf[1] & 0b0000'1111);
}

void sese::net::dns::DNSUtil::encodeFrameHeaderInfo(uint8_t buf[2], const sese::net::dns::FrameHeaderInfo &info) noexcept {
    buf[0] = info.transactionId / 0x100;
    buf[1] = info.transactionId % 0x100;
    encodeFrameFlagsInfo(&buf[2], info.flags);
    buf[4] = info.questions / 0x100;
    buf[5] = info.questions % 0x100;
    buf[6] = info.answerPrs / 0x100;
    buf[7] = info.answerPrs % 0x100;
    buf[8] = info.authorityPrs / 0x100;
    buf[9] = info.authorityPrs % 0x100;
    buf[10] = info.additionalPrs / 0x100;
    buf[11] = info.additionalPrs % 0x100;
}

void sese::net::dns::DNSUtil::encodeFrameFlagsInfo(uint8_t buf[2], const FrameFlagsInfo &info) noexcept {
    buf[0] = info.QR << 7;
    buf[0] |= info.opcode << 3;
    buf[0] |= info.AA << 2;
    buf[0] |= info.TC << 1;
    buf[0] |= info.RD;

    buf[1] = info.RA << 7;
    buf[1] |= info.z << 4;
    buf[1] |= info.rcode;
}

#define ASSERT_READ(buf, s)         \
    if (input->read(buf, s) == s) { \
        size += s;                  \
    } else {                        \
        return false;               \
    }                               \
    SESE_MARCO_END

bool sese::net::dns::DNSUtil::decodeQueries(size_t qcount, sese::InputStream *input, std::vector<Query> &vector) noexcept {
    while (qcount) {
        std::string result;
        bool first = true;
        uint8_t l;
        uint32_t offset = 12;
        uint32_t size = 12;
        char buf[128];
        while (true) {
            ASSERT_READ(&l, 1);
            // input->read(&l, 1);
            if (l == 0) {
                break;
            }
            ASSERT_READ(buf, l);
            // input->read(buf, l);
            if (first) {
                buf[l] = 0;
                result += buf;
                first = false;
            } else {
                buf[l] = 0;
                result += '.';
                result += buf;
            }
        }
        uint16_t type;
        ASSERT_READ(&type, sizeof(type));
        // input->read(&type, 2);
        type = FromBigEndian16(type);
        uint16_t class_;
        ASSERT_READ(&class_, sizeof(class_));
        // input->read(&class_, 2);
        class_ = FromBigEndian16(class_);

        vector.emplace_back(result, type, class_, offset);
        offset += size;
        qcount -= 1;
    }
    return true;
}

void sese::net::dns::DNSUtil::encodeQueries(sese::OutputStream *output, std::vector<Query> &vector) noexcept {
    for (auto &item: vector) {
        auto v = text::StringBuilder::split(item.getName(), ".");
        for (auto &i: v) {
            auto l = (uint8_t) i.length();
            output->write(&l, 1);
            output->write(i.c_str(), i.length());
        }
        output->write("\0", 1);

        uint16_t value = item.getType();
        value = ToBigEndian16(value);
        output->write(&value, sizeof(value));
        value = item.getClass();
        value = ToBigEndian16(value);
        output->write(&value, sizeof(value));
    }
}

void sese::net::dns::DNSUtil::encodeAnswers(OutputStream *output, std::vector<Answer> &vector) noexcept {
    for (auto &item: vector) {
        if (item.isRef()) {
            output->write(item.getName().c_str(), 2);
        } else {
            auto v = text::StringBuilder::split(item.getName(), ".");
            for (auto &i: v) {
                auto l = (uint8_t) i.length();
                output->write(&l, 1);
                output->write(i.c_str(), i.length());
            }
            output->write("\0", 1);
        }

        uint16_t v16 = item.getType();
        v16 = ToBigEndian16(v16);
        output->write(&v16, sizeof(v16));

        v16 = item.getClass();
        v16 = ToBigEndian16(v16);
        output->write(&v16, sizeof(v16));

        uint32_t v32 = item.getLiveTime();
        v32 = ToBigEndian32(v32);
        output->write(&v32, sizeof(v32));

        v16 = item.getLength();
        v16 = ToBigEndian16(v16);
        output->write(&v16, sizeof(v16));

        output->write(item.getAddress().c_str(), item.getLength());
    }
}

bool sese::net::dns::DNSUtil::decodeDomain(sese::InputStream *input, std::string &domain) noexcept {
    std::string result;
    bool first = true;
    uint8_t l;
    uint32_t size = 12;
    char buf[128];
    while (true) {
        ASSERT_READ(&l, 1);
        // input->read(&l, 1);
        if (l == 0) {
            break;
        }
        ASSERT_READ(buf, l);
        // input->read(buf, l);
        if (first) {
            buf[l] = 0;
            result += buf;
            first = false;
        } else {
            buf[l] = 0;
            result += '.';
            result += buf;
        }
    }
    domain = result;
    return true;
}

bool sese::net::dns::DNSUtil::decodeAnswers(size_t acount, sese::InputStream *input, std::vector<Answer> &vector) noexcept {
    while (acount) {
        std::string result;
        bool isRef = false;
        bool first = true;
        uint8_t l;
        uint32_t offset = 12;
        uint32_t size = 12;
        char buf[128];
        while (true) {
            ASSERT_READ(&l, 1);
            // input->read(&l, 1);

            if (l == 0) {
                break;
            }

            if (((l & 0b1100'0000) >> 6) == 3) {
                // 使用索引
                uint8_t tmp[2];
                tmp[0] = l & 0b0011'1111;
                ASSERT_READ(&tmp[1], 1);
                result = {(const char *) tmp, 2};
                isRef = true;
                break;
            }

            ASSERT_READ(buf, l);
            // input->read(buf, l);
            if (first) {
                buf[l] = 0;
                result += buf;
                first = false;
            } else {
                buf[l] = 0;
                result += '.';
                result += buf;
            }
        }

        uint16_t type;
        ASSERT_READ(&type, sizeof(type));
        // input->read(&type, 2);
        type = FromBigEndian16(type);
        uint16_t class_;
        ASSERT_READ(&class_, sizeof(class_));
        // input->read(&class_, 2);
        class_ = FromBigEndian16(class_);

        uint32_t time;
        ASSERT_READ(&time, sizeof(time));
        time = FromBigEndian32(time);

        uint16_t length;
        ASSERT_READ(&length, sizeof(length));
        length = FromBigEndian16(length);

        uint8_t data[16];
        ASSERT_READ(data, length);

        auto str = std::string((const char *) data, length);
        vector.emplace_back(isRef, result, type, class_, time, length, str);
        offset += size;

        acount -= 1;
    }

    return true;
}