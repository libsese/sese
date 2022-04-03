#include "net/Address.h"
#include "net/IPv4Address.h"
#ifdef _WIN32
struct NetInitStruct {
    NetInitStruct() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
    ~NetInitStruct() {
        WSACleanup();
    }
} netInitStruct; /* NOLINT */
#endif

sese::Address::Ptr sese::Address::create(const sockaddr *address, socklen_t addressLen) {
    if (address->sa_family == AF_INET) {
        return std::make_shared<IPv4Address>(*(const sockaddr_in *) address);
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
