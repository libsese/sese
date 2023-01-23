#include "sese/net/Address.h"
#include "sese/net/IPv6Address.h"

sese::Address::Ptr sese::Address::create(const sockaddr *address, socklen_t addressLen) {
    if (address->sa_family == AF_INET) {
        return std::make_shared<IPv4Address>(*(const sockaddr_in *) address);
    } else if (address->sa_family == AF_INET6) {
        return std::make_shared<IPv6Address>(*(const sockaddr_in6 *) address);
    } else {
        return nullptr;
    }
}

bool sese::Address::lookUp(std::vector<Address::Ptr> &result, const std::string &host, int family, int type, int protocol) {
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
        result.emplace_back(create(next->ai_addr, (socklen_t) next->ai_addrlen));
        next = next->ai_next;
    }

    freeaddrinfo(res);
    return true;
}

sese::Address::Ptr sese::Address::lookUpAny(const std::string &host, int family, int type, int protocol) {
    std::vector<Address::Ptr> temp;
    lookUp(temp, host, family, type, protocol);
    if (temp.empty()) {
        return nullptr;
    } else {
        return temp[0];
    }
}
