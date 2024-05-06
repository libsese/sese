#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <array>
#include <sese/security/evp/DES.h>

#include <openssl/evp.h>
#include <openssl/err.h>

#define CTX static_cast<EVP_CIPHER_CTX *>(this->ctx)

// GCOVR_EXCEL_START

inline auto switchCipher(sese::security::evp::DESEncrypter::Type type) {
    using Type = sese::security::evp::DESEncrypter::Type;
    switch (type) {
        case Type::ECB:
        default:
            return EVP_des_ecb();
        case Type::CBC:
            return EVP_des_cbc();
        case Type::CFB1:
            return EVP_des_cfb1();
        case Type::CFB8:
            return EVP_des_cfb8();
        case Type::CFB64:
            return EVP_des_cfb64();
        case Type::EDE:
            return EVP_des_ede();
        case Type::EDE3:
            return EVP_des_ede3();
    }
}

sese::security::evp::DESEncrypter::DESEncrypter(Type type, const std::array<unsigned char, 8> &key) { // NOLINT
    ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = switchCipher(type);
    memcpy(this->key, key.data(), 8);
    EVP_EncryptInit(CTX, cipher, this->key, nullptr);
}

sese::security::evp::DESEncrypter::DESEncrypter(Type type, const std::string &key) { // NOLINT
    ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = switchCipher(type);

    EVP_BytesToKey(
            cipher, EVP_md5(),
            nullptr,
            reinterpret_cast<const unsigned char *>(key.c_str()),
            static_cast<int>(key.length()),
            1,
            this->key, nullptr
    );

    EVP_EncryptInit(CTX, cipher, this->key, nullptr);
}

sese::security::evp::DESEncrypter::~DESEncrypter() {
    EVP_CIPHER_CTX_free(CTX);
}

int sese::security::evp::DESEncrypter::update(void *out, int &out_len, const void *in, int in_len) const noexcept {
    return EVP_EncryptUpdate(CTX, static_cast<unsigned char *>(out), &out_len, static_cast<const unsigned char *>(in), in_len);
}

int sese::security::evp::DESEncrypter::final(void *out, int &out_len) const noexcept {
    return EVP_EncryptFinal(CTX, static_cast<unsigned char *>(out), &out_len);
}

sese::security::evp::DESDecrypter::DESDecrypter(Type type, const std::array<unsigned char, 8> &key) { // NOLINT
    ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = switchCipher(type);
    memcpy(this->key, key.data(), 8);
    EVP_DecryptInit(CTX, cipher, this->key, nullptr);
}

sese::security::evp::DESDecrypter::DESDecrypter(Type type, const std::string &key) { // NOLINT
    ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = switchCipher(type);

    EVP_BytesToKey(
            cipher, EVP_md5(),
            nullptr,
            reinterpret_cast<const unsigned char *>(key.c_str()),
            static_cast<int>(key.length()),
            1,
            this->key, nullptr
    );

    EVP_DecryptInit(CTX, cipher, this->key, nullptr);
}

sese::security::evp::DESDecrypter::~DESDecrypter() {
    EVP_CIPHER_CTX_free(CTX);
}

int sese::security::evp::DESDecrypter::update(void *out, int &out_len, const void *in, int in_len) const noexcept {
    return EVP_DecryptUpdate(CTX, static_cast<unsigned char *>(out), &out_len, static_cast<const unsigned char *>(in), in_len);
}

int sese::security::evp::DESDecrypter::final(void *out, int &out_len) const noexcept {
    return EVP_DecryptFinal(CTX, static_cast<unsigned char *>(out), &out_len);
}

// GCOVR_EXCEL_STOP

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
