#include "sese/net/IPAddress.h"

using namespace sese::net;

IPAddress::Ptr IPAddress::create(const char *address, uint16_t port) {
    addrinfo *res, hints{0};
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    auto err = getaddrinfo(address, nullptr, &hints, &res);
    if (err) {
        return nullptr;
    }

    // GCOVR_EXCL_START
    IPAddress::Ptr result = std::dynamic_pointer_cast<IPAddress>(Address::create(res->ai_addr, static_cast<socklen_t>(res->ai_addrlen)));
    if (result) {
        result->setPort(port);
    }
    else {
        result = nullptr;
    }
    // GCOVR_EXCL_STOP

    freeaddrinfo(res);
    return result;
}
