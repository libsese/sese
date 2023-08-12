#include <sese/net/dns/Client.h>
#include <sese/net/Socket.h>
#include <sese/net/dns/DNSUtil.h>
#include <sese/net/dns/DNSSession.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/util/Random.h>

#include <random>

sese::net::Address::Ptr sese::net::dns::Client::resolveSystem(const std::string &domain, int family, int type, int protocol) noexcept {
    return Address::lookUpAny(domain, family, type, protocol);
}

sese::net::Address::Ptr sese::net::dns::Client::resolveCustom(const std::string &domain, const IPAddress::Ptr &server, int family, int type, int protocol) noexcept {
    auto sock = sese::net::Socket::socket(server->getFamily(), SOCK_DGRAM, IPPROTO_IP);
    auto socket = sese::net::Socket(sock, nullptr);
    auto expectType = family == AF_INET ? SESE_DNS_QR_TYPE_A : SESE_DNS_QR_TYPE_AAAA;

    sese::Random device;
    auto engine = std::default_random_engine(device());
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

    DNSSession session;
    session.getQueries().emplace_back(domain, expectType, SESE_DNS_QR_CLASS_IN, 0);

    uint8_t buffer[512];
    auto output = sese::OutputBufferWrapper((char *) buffer + 12, sizeof(buffer) - 12);

    DNSUtil::encodeFrameHeaderInfo(buffer, info);
    DNSUtil::encodeQueries(&output, session.getQueries());

    socket.send(buffer, 12 + output.getLen(), server, 0);

    auto len = socket.recv(buffer, sizeof(buffer), server, 0);
    auto input = sese::InputBufferWrapper((const char *) buffer + 12, len - 12);

    session.getQueries().clear();
    DNSUtil::decodeFrameHeaderInfo(buffer, info);

    if (info.flags.rcode != SESE_DNS_RCODE_NO_ERROR) {
        return nullptr;
    }

    DNSUtil::decodeQueries(info.questions, &input, session.getQueries());
    DNSUtil::decodeAnswers(info.answerPrs, &input, session.getAnswers());

    for (auto &item: session.getAnswers()) {
        std::string name;
        if (item.isRef()) {
            uint16_t index;
            memcpy(&index, item.getName().c_str(), 2);
            index = FromBigEndian16(index);
            auto in = sese::InputBufferWrapper((const char *) buffer + index, sizeof(buffer) - index);
            DNSUtil::decodeDomain(&in, name);
        } else {
            name = item.getName();
        }
        if (expectType == item.getType()) {
            if (family == AF_INET) {
                auto data = (const uint32_t *) item.getAddress().c_str();
                uint32_t addr = FromBigEndian32(*data);
                return std::make_shared<sese::net::IPv4Address>(addr, 0);
            } else {
                auto data = (uint8_t *) item.getAddress().c_str();
                return std::make_shared<sese::net::IPv6Address>(data, 0);
            }
        }
    }

    return nullptr;
}

sese::net::Address::Ptr sese::net::dns::Client::resolveAuto(const std::string &domain, IPAddress::Ptr server, int family, int type, int protocol) noexcept {
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