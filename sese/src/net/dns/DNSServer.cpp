#include <sese/net/dns/DNSServer.h>
#include <sese/net/dns/DNSUtil.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/util/OutputBufferWrapper.h>

#include <sese/record/Marco.h>

sese::net::dns::DNSServer::Ptr sese::net::dns::DNSServer::create(const sese::net::dns::DNSConfig *config) noexcept {
    auto socket = std::make_shared<sese::net::Socket>(Socket::Family::IPv4, Socket::Type::UDP, IPPROTO_IP);
    if (socket->bind(config->address)) {
        return nullptr;
    }

    auto ptr = new DNSServer;
    ptr->socket = socket;

    for (auto &item: config->hostMap) {
        auto v = text::StringBuilder::split(item.second, ".");
        if (v.size() != 4) {
            continue;
        }

        char *end;
        uint8_t buffer[4];
        for (int i = 0; i < 4; ++i) {
            buffer[i] = (uint8_t) std::strtol(v[i].c_str(), &end, 10);
        }
        ptr->hostMap[item.first] = std::string((const char *) buffer, 4);
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
                auto iterator = hostMap.find(q.getName());
                if (iterator == hostMap.end()) {
                    info.flags.rcode = SESE_DNS_RCODE_NAME_ERROR;
                    continue;
                } else {
                    uint16_t offset = q.getOffset();
                    offset |= 0b1100'0000'0000'0000;
                    offset = ToBigEndian16(offset);
                    std::string name = {(const char *) &offset, 4};
                    session.getAnswers().emplace_back(
                            true,
                            name,
                            SESE_DNS_QR_TYPE_A,
                            SESE_DNS_QR_CLASS_IN,
                            600,
                            4,
                            (const uint8_t *) iterator->second.c_str()
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
        socket->send(buffer, 12 + output.getLen(), clientAddress, 0);
    }
}