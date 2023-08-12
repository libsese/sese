#pragma once

#include <sese/net/Socket.h>
#include <sese/net/dns/Client.h>
#include <sese/net/dns/DNSSession.h>
#include <sese/thread/Thread.h>

#include <atomic>
#include <map>

namespace sese::net::dns {

    struct API DNSConfig {
        IPAddress::Ptr address;
        std::map<std::string, std::string> hostIPv4Map;
        std::map<std::string, std::string> hostIPv6Map;
    };

    class API DNSServer {
    public:
        using Ptr = std::unique_ptr<DNSServer>;

        static DNSServer::Ptr create(const DNSConfig *config) noexcept;

        ~DNSServer() noexcept;

        void start() noexcept;

        void shutdown() noexcept;

    private:
        DNSServer() = default;

        void loop() noexcept;

        std::atomic_bool isShutdown{false};
        Socket::Ptr socket = nullptr;
        Thread::Ptr thread = nullptr;
        std::map<std::string, std::string> hostIPv4Map;
        std::map<std::string, std::string> hostIPv6Map;
    };
}