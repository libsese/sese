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

#include <openssl/ssl.h>

#include <sese/security/SSLContext.h>
#include <sese/util/Util.h>

sese::security::SecuritySocket::SecuritySocket(std::shared_ptr<SSLContext> context, Socket::Family family, int32_t flags) noexcept
    : Socket(family, Socket::Type::TCP, flags),
      context(std::move(context)) {
}

sese::security::SecuritySocket::SecuritySocket(std::shared_ptr<SSLContext> context, void *ssl, socket_t fd) noexcept : Socket(fd, nullptr) {
    this->context = std::move(context);
    this->ssl = ssl;
}

int32_t sese::security::SecuritySocket::connect(Address::Ptr address) noexcept {
    while (true) {
        auto rt = Socket::connect(address);
        // GCOVR_EXCL_START
        if (rt != 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EALREADY || err == EINPROGRESS) {
                sese::sleep(0);
                continue;
            } else if (err == EISCONN) {
                break;
            } else {
                return rt;
            }
        } else {
            break;
        }
        // GCOVR_EXCL_STOP
    }

    ssl = SSL_new(static_cast<SSL_CTX *>(context->getContext()));
    auto client_fd = this->getRawSocket();

    SSL_set_fd(static_cast<SSL *>(ssl), static_cast<int>(client_fd));
    SSL_set_connect_state(static_cast<SSL *>(ssl));
    // GCOVR_EXCL_START
    while (true) {
        auto rt = SSL_do_handshake(static_cast<SSL *>(ssl));
        if (rt <= 0) {
            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
            auto err = SSL_get_error(static_cast<SSL *>(ssl), rt);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                SSL_free(static_cast<SSL *>(ssl));
                Socket::close();
                return -1;
            }
        } else {
            break;
        }
    }
    // GCOVR_EXCL_STOP

    return 0;
}

sese::net::Socket::Ptr sese::security::SecuritySocket::accept() const {
    auto client_fd = ::accept(this->getRawSocket(), nullptr, nullptr);
    // GCOVR_EXCL_START
    if (client_fd == -1) {
        return nullptr;
    }
    // GCOVR_EXCL_STOP

    auto client_ssl = SSL_new(static_cast<SSL_CTX *>(context->getContext()));
    SSL_set_fd(client_ssl, static_cast<int>(client_fd));
    SSL_set_accept_state(client_ssl);
    // GCOVR_EXCL_START
    while (true) {
        auto rt = SSL_do_handshake(client_ssl);
        if (rt <= 0) {
            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
            auto err = SSL_get_error(client_ssl, rt);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                SSL_free(client_ssl);
                Socket::close(client_fd);
                return nullptr;
            }
        } else {
            break;
        }
    }
    // GCOVR_EXCL_STOP

    return std::make_shared<SecuritySocket>(context, client_ssl, client_fd);
}

int32_t sese::security::SecuritySocket::shutdown(sese::net::Socket::ShutdownMode mode) const {
    return 0;
}

int64_t sese::security::SecuritySocket::read(void *buffer, size_t length) {
    return SSL_read(static_cast<SSL *>(ssl), buffer, static_cast<int>(length));
}

int64_t sese::security::SecuritySocket::write(const void *buffer, size_t length) {
    return SSL_write(static_cast<SSL *>(ssl), buffer, static_cast<int>(length));
}

void sese::security::SecuritySocket::close() {
    if (ssl) {
        SSL_shutdown(static_cast<SSL *>(ssl));
        SSL_free(static_cast<SSL *>(ssl));
    }
    Socket::close();
}