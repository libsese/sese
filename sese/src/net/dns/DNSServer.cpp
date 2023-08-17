#include <sese/net/dns/DNSServer.h>
#include <sese/net/dns/DNSUtil.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/util/Util.h>

sese::net::dns::DNSServer::Ptr sese::net::dns::DNSServer::create(const sese::net::dns::DNSConfig *config) noexcept {
    auto socket = std::make_shared<sese::net::Socket>(Socket::Family::IPv4, Socket::Type::UDP, IPPROTO_IP);
    if (socket->bind(config->address)) {
        return nullptr;
    }

    socket->setNonblocking();

    auto ptr = new DNSServer;
    ptr->socket = socket;

    for (auto &item: config->hostIPv4Map) {
        // auto v = text::StringBuilder::split(item.second, ".");
        // if (v.size() != 4) {
        //     continue;
        // }
        // char *end;
        // uint8_t buffer[4];
        // for (int i = 0; i < 4; ++i) {
        //     buffer[i] = (uint8_t) std::strtol(v[i].c_str(), &end, 10);
        // }
        uint8_t buffer[4];
        inet_pton(AF_INET, item.second.c_str(), buffer);
        ptr->hostIPv4Map[item.first] = std::string((const char *) buffer, 4);
    }

    for (auto &item: config->hostIPv6Map) {
        uint8_t buffer[16];
        inet_pton(AF_INET6, item.second.c_str(), buffer);
        ptr->hostIPv6Map[item.first] = std::string((const char *) buffer, 16);
    }

    return std::unique_ptr<DNSServer>(ptr);
}

sese::net::dns::DNSServer::~DNSServer() noexcept {
    if (thread && !isShutdown) {
        shutdown();
        thread = nullptr;
    }

    if (socket) {
        socket->close();
        socket = nullptr;
    }
}

void sese::net::dns::DNSServer::start() noexcept {
    this->thread = std::make_unique<Thread>([this] { this->loop(); }, "DNS");
    this->thread->start();
}

void sese::net::dns::DNSServer::shutdown() noexcept {
    if (thread) {
        isShutdown = true;

        thread->join();
        socket->close();

        thread = nullptr;
        socket = nullptr;
    }
}

void sese::net::dns::DNSServer::loop() noexcept {
    auto clientAddress = std::make_shared<sese::net::IPv4Address>();
    while (true) {
        if (isShutdown) {
            break;
        }

        uint8_t buffer[512];
        auto len = socket->recv(buffer, sizeof(buffer), clientAddress, 0);
        if (len <= 0) {
            sese::sleep(0);
            continue;
        }

        auto input = sese::InputBufferWrapper((const char *) buffer + 12, len - 12);
        auto output = sese::OutputBufferWrapper((char *) buffer + 12, sizeof(buffer) - 12);

        FrameHeaderInfo info;
        DNSSession session;
        DNSUtil::decodeFrameHeaderInfo(buffer, info);
        DNSUtil::decodeQueries(info.questions, &input, session.getQueries());

        info.flags.rcode = SESE_DNS_RCODE_NO_ERROR;
        info.flags.RD = 0;
        info.flags.QR = 1;
        for (auto &q: session.getQueries()) {
            if (q.getType() == SESE_DNS_QR_TYPE_A) {
                auto iterator = hostIPv4Map.find(q.getName());
                if (iterator == hostIPv4Map.end()) {
                    info.flags.rcode = SESE_DNS_RCODE_NAME_ERROR;
                    continue;
                } else {
                    uint16_t offset = q.getOffset();
                    offset |= 0b1100'0000'0000'0000;
                    offset = ToBigEndian16(offset);
                    std::string name = {(const char *) &offset, 2};
                    session.getAnswers().emplace_back(
                            true,
                            name,
                            SESE_DNS_QR_TYPE_A,
                            SESE_DNS_QR_CLASS_IN,
                            600,
                            4,
                            iterator->second
                    );
                }
                info.answerPrs += 1;
            } else if (q.getType() == SESE_DNS_QR_TYPE_AAAA) {
                auto iterator = hostIPv6Map.find(q.getName());
                if (iterator == hostIPv6Map.end()) {
                    info.flags.rcode = SESE_DNS_RCODE_NAME_ERROR;
                    continue;
                } else {
                    uint16_t offset = q.getOffset();
                    offset |= 0b1100'0000'0000'0000;
                    offset = ToBigEndian16(offset);
                    std::string name = {(const char *) &offset, 2};
                    session.getAnswers().emplace_back(
                            true,
                            name,
                            SESE_DNS_QR_TYPE_AAAA,
                            SESE_DNS_QR_CLASS_IN,
                            600,
                            16,
                            iterator->second
                    );
                }
                info.answerPrs += 1;
            } else {
                info.flags.rcode = SESE_DNS_RCODE_NOT_IMPLEMENTED;
                continue;
            }
        }

        DNSUtil::encodeFrameHeaderInfo(buffer, info);
        DNSUtil::encodeQueries(&output, session.getQueries());
        DNSUtil::encodeAnswers(&output, session.getAnswers());
        socket->send(buffer, 12 + output.getLength(), clientAddress, 0);
    }
}