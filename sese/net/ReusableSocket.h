// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file ReusableSocket.h
/// \author kaoru
/// \brief Reusable socket builder for address and port
/// \date June 2, 2023
/// \version 0.1

#pragma once

#include <sese/net/Socket.h>
#include <sese/net/IPv6Address.h>
#include <sese/security/SecuritySocket.h>

#include <optional>

namespace sese::net {

/// \brief Reusable socket builder for address and port
/// \warning The behavior of sockets built with this builder is inconsistent across platforms <p>
/// Linux: The kernel handles load balancing, and multiple sockets can receive connections <p>
/// Darwin: No load balancing, only the last socket can receive connections <p>
/// Windows: No load balancing, only the first socket can receive connections
/// \see https://www.cnblogs.com/xybaby/p/7341579.html
class ReusableSocket { // GCOVR_EXCL_LINE
public:
    /// Initialize the reusable socket template for address and port
    /// \param address The address
    /// \param type The type
    explicit ReusableSocket(IPAddress::Ptr address, Socket::Type type = Socket::Type::TCP);

    /// Build a native socket according to the template
    /// \retval -1 Creation failed
    /// \return Native socket
    socket_t makeRawSocket() noexcept;

    /// Build a sese::net::Socket according to the template
    /// \retval nullptr Creation failed
    /// \return sese::net::Socket
    std::optional<Socket> makeSocket() noexcept;

protected:
    std::optional<Socket> builtinMakeSocket() noexcept;

protected:
    IPAddress::Ptr addr{};
    Socket::Type type{};
};

} // namespace sese::net