#pragma once

#include <sese/net/Socket.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SecuritySocket.h>

#include <optional>

namespace sese::net {

    class ReusableSocket {
    public:
        explicit ReusableSocket(IPAddress::Ptr address, Socket::Type type = Socket::Type::TCP);

        socket_t makeRawSocket() noexcept;

        std::optional<Socket> makeSocket() noexcept;

    protected:
        std::optional<Socket> builtinMakeSocket() noexcept;

    protected:
        IPAddress::Ptr addr{};
        Socket::Type type{};
    };

}