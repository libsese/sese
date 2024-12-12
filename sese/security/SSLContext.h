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

/// \file SSLContext.h
/// \brief SSL context
/// \version 0.1
/// \author kaoru
/// \date July 25, 2023

#pragma once

#include <sese/security/SecuritySocket.h>
#include <sese/util/ErrorCode.h>

namespace sese::security {

/// SSL context
class SSLContext final : public std::enable_shared_from_this<SSLContext> {
public:
    using Ptr = std::shared_ptr<SSLContext>;
    using Socket = sese::net::Socket;

    explicit SSLContext(const void *method) noexcept;
    ~SSLContext() noexcept;

    [[nodiscard]] void *getContext() const noexcept;

    /// \brief Load certificate from file
    /// \param file Path to the certificate file
    /// \return Loading result
    bool importCertFile(const char *file) const noexcept;
    /// \brief Load private key from file
    /// \note This function implements via `SSL_CTX_use_PrivateKey_file`, it will also verify the certificate
    /// \param file Path to the private key file
    /// \return Loading result
    bool importPrivateKeyFile(const char *file) const noexcept;
    /// \brief Verify certificate and private key
    /// \return Verification result
    bool authPrivateKey() const noexcept;

    /// \brief Create a TCP socket from the current context
    /// \param family Protocol family
    /// \param flags Flags
    /// \return The created socket
    Socket::Ptr newSocketPtr(Socket::Family family, int32_t flags);

    /// \brief Release ownership of the current object
    /// \warning Note: This function is used to address asio::ssl::context not following the
    /// who allocates who releases principle. After calling this function, the object will lose control of SSL_CTX.
    /// Unless you are very sure of what you are doing, do not use this function.
    /// If you only need to obtain SSL_CTX without changing its lifecycle, please use getContext.
    /// \return Underlying SSL_CTX pointer
    void *release() noexcept;

    /// \brief Deep copy the current context, including certificates and private keys (must exist),
    /// with independent lifecycle
    /// \return SSL context
    std::unique_ptr<SSLContext> copy() const noexcept;

    static ErrorCode getErrorCode()  noexcept;

private:
    SSLContext() = default;

    // SSL_CTX *context = nullptr;
    void *context = nullptr;
};

} // namespace sese::security