#include <sese/security/evp/Crypter.h>

#include <openssl/evp.h>

using namespace sese::security::evp;

Decrypter::Decrypter(const CrypterContext::Ptr &crypter_context) : crypter_context_(crypter_context) {
    ctx_ = EVP_CIPHER_CTX_new();

    EVP_DecryptInit(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<EVP_CIPHER *>(crypter_context_->crypter_pointer),
            reinterpret_cast<const unsigned char *>(crypter_context_->key.getBuffer()),
            reinterpret_cast<const unsigned char *>(crypter_context_->vector.getBuffer())
    );
}

int Decrypter::update(void *out, int &out_len, const void *in, int in_len) const noexcept {
    return EVP_DecryptUpdate(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<unsigned char *>(out),
            &out_len,
            static_cast<const unsigned char *>(in),
            in_len
    );
}

int Decrypter::final(void *out, int &out_len) const noexcept {
    return EVP_DecryptFinal(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<unsigned char *>(out),
            &out_len
    );
}

Decrypter::~Decrypter() {
    EVP_CIPHER_CTX_free(static_cast<EVP_CIPHER_CTX *>(ctx_));
}

Encrypter::Encrypter(const CrypterContext::Ptr &crypter_context) : crypter_context_(crypter_context) {
    ctx_ = EVP_CIPHER_CTX_new();

    EVP_EncryptInit(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<EVP_CIPHER *>(crypter_context_->crypter_pointer),
            reinterpret_cast<const unsigned char *>(crypter_context_->key.getBuffer()),
            reinterpret_cast<const unsigned char *>(crypter_context_->vector.getBuffer())
    );
}

int Encrypter::update(void *out, int &out_len, const void *in, int in_len) const noexcept {
    return EVP_EncryptUpdate(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<unsigned char *>(out),
            &out_len,
            static_cast<const unsigned char *>(in),
            in_len
    );
}

int Encrypter::final(void *out, int &out_len) const noexcept {
    return EVP_EncryptFinal(
            static_cast<EVP_CIPHER_CTX *>(ctx_),
            static_cast<unsigned char *>(out),
            &out_len
    );
}

Encrypter::~Encrypter() {
    EVP_CIPHER_CTX_free(static_cast<EVP_CIPHER_CTX *>(ctx_));
}