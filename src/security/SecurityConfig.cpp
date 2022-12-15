#include <openssl/ssl.h>

#include <sese/security/SecurityConfig.h>

int32_t sese::security::SecurityInitTask::init() noexcept {
    SSL_library_init();
    SSLeay_add_all_algorithms();
    SSL_load_error_strings();
    return 0;
}
