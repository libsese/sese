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

/// \file SecuritySocket.h
/// \brief Security socket class
/// \author kaoru
/// \version 0.2
/// \date July 25, 2023

#pragma once

#include <sese/net/Socket.h>

namespace sese::security {

class  SSLContext;

/// Security socket class
class SecuritySocket final : public sese::net::Socket {
public:
    using Socket = sese::net::Socket;
    using Address = sese::net::Address;

    SecuritySocket(std::shared_ptr<SSLContext> context, Socket::Family family, int32_t flags) noexcept;
    SecuritySocket(std::shared_ptr<SSLContext> context, void *ssl, socket_t fd) noexcept;

    int32_t connect(Address::Ptr address) noexcept override;
    [[nodiscard]] Socket::Ptr accept() const override;
    [[nodiscard]] int32_t shutdown(ShutdownMode mode) const override;
    int64_t read(void *buffer, size_t length) override;
    int64_t write(const void *buffer, size_t length) override;
    void close() override;

    [[nodiscard]] void *getSSL() const { return ssl; }

private:
    std::shared_ptr<SSLContext> context;
    void *ssl = nullptr;
};
} // namespace sese::security
