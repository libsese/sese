#include <openssl/ssl.h>

#include <sese/security/SSLContextBuilder.h>

using namespace sese::security;

SSLContext::Ptr SSLContextBuilder::SSL4Client() noexcept {
    auto *method = TLS_client_method();
    return std::make_shared<SSLContext>(method);
}

SSLContext::Ptr SSLContextBuilder::SSL4Server() noexcept {
    auto method = TLS_server_method();
    return std::make_shared<SSLContext>(method);
}