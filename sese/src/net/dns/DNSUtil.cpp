#include <sese/net/dns/DNSUtil.h>

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