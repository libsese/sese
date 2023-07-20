#include <openssl/ssl.h>

#include <sese/security/SSLContext.h>
#include <sese/record/Marco.h>

sese::security::SecuritySocket::SecuritySocket(std::shared_ptr<SSLContext> context, Socket::Family family, int32_t flags) noexcept
    : Socket(family, Socket::Type::TCP, flags),
      context(std::move(context)) {
}

sese::security::SecuritySocket::SecuritySocket(std::shared_ptr<SSLContext> context, void *ssl, socket_t fd) noexcept : Socket(fd, nullptr) {
    this->context = std::move(context);
    this->ssl = ssl;
}

int32_t sese::security::SecuritySocket::connect(Address::Ptr address) noexcept {
    auto rt = Socket::connect(address);
    // GCOVR_EXCL_START
    if (rt != 0) {
        return rt;
    }
    // GCOVR_EXCL_STOP

    ssl = SSL_new((SSL_CTX *) context->getContext());
    auto clientFd = this->getRawSocket();

    SSL_set_fd((SSL *) ssl, (int) clientFd);
    SSL_set_connect_state((SSL *) ssl);
    // GCOVR_EXCL_START
    while (true) {
        rt = SSL_do_handshake((SSL *) ssl);
        if (rt <= 0) {
            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
            auto err = SSL_get_error((SSL *) ssl, rt);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                SSL_free((SSL *) ssl);
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
    auto clientFd = ::accept(this->getRawSocket(), nullptr, nullptr);
    // GCOVR_EXCL_START
    if (clientFd == -1) {
        return nullptr;
    }
    // GCOVR_EXCL_STOP

    auto clientSSL = SSL_new((SSL_CTX *) context->getContext());
    SSL_set_fd(clientSSL, (int) clientFd);
    SSL_set_accept_state(clientSSL);
    // GCOVR_EXCL_START
    while (true) {
        auto rt = SSL_do_handshake(clientSSL);
        if (rt <= 0) {
            // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
            auto err = SSL_get_error(clientSSL, rt);
            if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                SSL_free(clientSSL);
                Socket::close(clientFd);
                return nullptr;
            }
        } else {
            break;
        }
    }
    // GCOVR_EXCL_STOP

    return std::make_shared<SecuritySocket>(context, clientSSL, clientFd);
}

int32_t sese::security::SecuritySocket::shutdown(sese::net::Socket::ShutdownMode mode) const {
    return 0;
}

int64_t sese::security::SecuritySocket::read(void *buffer, size_t length) {
    return SSL_read((SSL *) ssl, buffer, (int) length);
}

int64_t sese::security::SecuritySocket::write(const void *buffer, size_t length) {
    return SSL_write((SSL *) ssl, buffer, (int) length);
}

void sese::security::SecuritySocket::close() {
    if (ssl) {
        SSL_shutdown((SSL *) ssl);
        SSL_free((SSL *) ssl);
    }
    Socket::close();
}