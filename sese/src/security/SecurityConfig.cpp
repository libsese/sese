#include <openssl/ssl.h>

#include <sese/security/SecurityConfig.h>

int32_t sese::security::SecurityInitTask::init() noexcept {
    SSL_library_init();
    SSL_load_error_strings();
    return 0;
}

int32_t sese::security::SecurityInitTask::destroy() noexcept {
    return 0;
}
