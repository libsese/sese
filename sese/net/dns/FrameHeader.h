/// \file FrameHeader.h
/// \author kaoru
/// \brief DNS 帧头信息
/// \date 2023年8月11日

#pragma once

#include <sese/Config.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::dns {

/// 帧标识信息
struct FrameFlagsInfo {
    uint8_t QR = 0;
    uint8_t opcode = 0;
    uint8_t AA = 0;
    uint8_t TC = 0;
    uint8_t RD = 0;
    uint8_t RA = 0;
    uint8_t z = 0;
    uint8_t rcode = 0;
};

/// 帧头信息
struct FrameHeaderInfo {
    uint16_t transactionId = 0;
    FrameFlagsInfo flags{};
    uint16_t questions = 0;
    uint16_t answerPrs = 0;
    uint16_t authorityPrs = 0;
    uint16_t additionalPrs = 0;
};
} // namespace sese::net::dns

#define SESE_DNS_RCODE_NO_ERROR 0
#define SESE_DNS_RCODE_FORMAT_ERROR 1
#define SESE_DNS_RCODE_SERVER_FAILURE 2
#define SESE_DNS_RCODE_NAME_ERROR 3
#define SESE_DNS_RCODE_NOT_IMPLEMENTED 4
#define SESE_DNS_RCODE_REFUSED 5

#define SESE_DNS_QR_TYPE_A 1
#define SESE_DNS_QR_TYPE_CNAME 5
#define SESE_DNS_QR_TYPE_PTR 12
#define SESE_DNS_QR_TYPE_HINFO 13
#define SESE_DNS_QR_TYPE_MX 15
#define SESE_DNS_QR_TYPE_AAAA 28
#define SESE_DNS_QR_TYPE_AXFR 252
#define SESE_DNS_QR_TYPE_ANY 255

#define SESE_DNS_QR_CLASS_IN 1