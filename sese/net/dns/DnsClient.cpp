#include <sese/net/dns/DnsClient.h>
#include <sese/net/Socket.h>
#include <sese/net/dns/DnsUtil.h>
#include <sese/net/dns/DnsSession.h>
#include <sese/io/InputBufferWrapper.h>
#include <sese/io/OutputBufferWrapper.h>

#include <random>

sese::net::Address::Ptr sese::net::dns::DnsClient::resolveSystem(const std::string &domain, int family, int type, int protocol) noexcept {
    return Address::lookUpAny(domain, family, type, protocol);
}

sese::net::Address::Ptr sese::net::dns::DnsClient::resolveCustom(const std::string &domain, const IPAddress::Ptr &server, int family, int type, int protocol) noexcept {
    auto sock = sese::net::Socket::socket(server->getFamily(), SOCK_DGRAM, IPPROTO_IP);
    auto socket = sese::net::Socket(sock, nullptr);
    auto expect_type = family == AF_INET ? SESE_DNS_QR_TYPE_A : SESE_DNS_QR_TYPE_AAAA;

    std::random_device device;
    auto engine = std::default_random_engine((uint32_t) device());
    std::uniform_int_distribution<uint16_t> dis(1, UINT16_MAX);

    auto id = dis(engine);

    sese::net::dns::FrameHeaderInfo info{};
    info.transactionId = id;
    info.flags.QR = 0;
    info.flags.TC = 0;
    info.flags.opcode = 0;
    info.flags.RD = 1;
    info.flags.RA = 0;
    info.flags.rcode = 0;
    info.flags.AA = 0;
    info.questions = 1;

    DnsSession session;
    session.getQueries().emplace_back(domain, expect_type, SESE_DNS_QR_CLASS_IN, 0);

    uint8_t buffer[DNS_PACKAGE_SIZE];
    auto output = sese::io::OutputBufferWrapper((char *) buffer + 12, sizeof(buffer) - 12);

    DnsUtil::encodeFrameHeaderInfo(buffer, info);
    DnsUtil::encodeQueries(&output, session.getQueries());

    socket.send(buffer, 12 + output.getLength(), server, 0);

    auto len = socket.recv(buffer, sizeof(buffer), server, 0);
    auto input = sese::io::InputBufferWrapper((const char *) buffer + 12, len - 12);

    session.getQueries().clear();
    DnsUtil::decodeFrameHeaderInfo(buffer, info);

    if (info.flags.rcode != SESE_DNS_RCODE_NO_ERROR) {
        return nullptr;
    }

    DnsUtil::decodeQueries(info.questions, &input, session.getQueries());
    DnsUtil::decodeAnswers(info.answerPrs, &input, session.getAnswers(), (const char *) buffer);

    for (auto &item: session.getAnswers()) {
        if (expect_type == item.getType()) {
            if (family == AF_INET) {
                auto data = (const uint32_t *) item.getData().c_str();
                uint32_t addr = FromBigEndian32(*data);
                return std::make_shared<sese::net::IPv4Address>(addr, 0);
            } else {
                auto data = (uint8_t *) item.getData().c_str();
                return std::make_shared<sese::net::IPv6Address>(data, 0);
            }
        }
    }

    return nullptr;
}

sese::net::Address::Ptr sese::net::dns::DnsClient::resolveAuto(const std::string &domain, const IPAddress::Ptr &server, int family, int type, int protocol) noexcept {
    auto result = resolveCustom(domain, server, family, type, protocol);
    if (result) {
        return result;
    }
    result = resolveSystem(domain, family, type, protocol);
    if (result) {
        return result;
    }
    return nullptr;
}