#include "sese/net/Address.h"
#include "sese/net/IPv6Address.h"

using namespace sese::net;

Address::Ptr Address::create(const sockaddr *address, socklen_t address_len) {
    if (address->sa_family == AF_INET) {
        return std::make_shared<IPv4Address>(*reinterpret_cast<const sockaddr_in *>(address));
    } else if (address->sa_family == AF_INET6) {
        return std::make_shared<IPv6Address>(*reinterpret_cast<const sockaddr_in6 *>(address));
    } else {
        return nullptr;
    }
}

bool Address::lookUp(std::vector<Address::Ptr> &result, const std::string &host, int family, int type, int protocol) {
    struct addrinfo *res, hints{0};
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    int err = getaddrinfo(host.c_str(), nullptr, &hints, &res);
    if (err) {
        return false;
    }

    auto next = res;
    while (next) {
        result.emplace_back(create(next->ai_addr, static_cast<socklen_t>(next->ai_addrlen)));
        next = next->ai_next;
    }

    freeaddrinfo(res);
    return true;
}

Address::Ptr Address::lookUpAny(const std::string &host, int family, int type, int protocol) {
    std::vector<Address::Ptr> temp;
    lookUp(temp, host, family, type, protocol);
    if (temp.empty()) {
        return nullptr;
    } else {
        return temp[0];
    }
}

int sese::net::inetPton(int af, const char *src, void *dst) noexcept {
#if defined(__MINGW32__) && !defined(InetPtonA)
    char ipaddr[64];
    int addr_size = -1;
    if (af == AF_INET) {
        struct sockaddr_in ip4 {};
        addr_size = sizeof(SOCKADDR_IN);
        WSAStringToAddress((LPSTR) ipaddr, af, nullptr, (LPSOCKADDR) &ip4, &addr_size);
        memcpy(dst, &(ip4.sin_addr), 4);
        return 1;
    } else if (af == AF_INET6) {
        struct sockaddr_in6 ip6 {};
        addr_size = sizeof(SOCKADDR_IN6);
        WSAStringToAddress((LPSTR) ipaddr, af, nullptr, (LPSOCKADDR) &ip6, &addr_size);
        memcpy(dst, &(ip6.sin6_addr), 16);
        return 1;
    }
    return 0;
#elif defined(__MINGW32__)
    return InetPtonA(af, src, dst);
#else
    return ::inet_pton(af, src, dst);
#endif
}
