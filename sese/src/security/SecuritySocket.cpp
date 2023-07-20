#include <openssl/ssl.h>
#include <openssl/err.h>

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
    if (rt != 0) {
        return rt;
    }

    ssl = SSL_new((SSL_CTX *) context->getContext());
    if (ssl == nullptr) {
        return -1;
    }

    rt = SSL_set_fd((SSL *) ssl, (int) getRawSocket());
    if (rt != 1) {
        return -1;
    }

    rt = SSL_connect((SSL *) ssl);
    if (rt != 1) {
        return -1;
    } else {
        return 0;
    }
}

sese::net::Socket::Ptr sese::security::SecuritySocket::accept() const {
    auto clientFd = ::accept(this->getRawSocket(), nullptr, nullptr);
    if (clientFd == -1) {
        return nullptr;
    }

    auto clientSSL = SSL_new((SSL_CTX *) context->getContext());
    SSL_set_fd(clientSSL, (int) clientFd);
    auto rt = SSL_accept(clientSSL);
    if (rt != 1) {
        char msg[1024];
        ERR_error_string_n(ERR_get_error(), msg, sizeof(msg));
        SESE_ERROR("%s", msg);
        SSL_free(clientSSL);
        Socket::close(clientFd);
        return nullptr;
    }

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
    SSL_shutdown((SSL *) ssl);
    SSL_free((SSL *) ssl);
    Socket::close();
}