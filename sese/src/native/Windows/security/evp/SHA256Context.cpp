#include <sese/security/evp/SHA256Context.h>

sese::security::evp::SHA256Context::SHA256Context() noexcept : Context() {
    CryptAcquireContext(&this->hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(this->hProv, CALG_SHA_256, 0, 0, &this->hHash);
}

sese::security::evp::SHA256Context::~SHA256Context() noexcept {
    if (this->hProv) {
        CryptReleaseContext(this->hProv, 0);
    }

    if (this->hHash) {
        CryptDestroyHash(this->hHash);
    }
}

void sese::security::evp::SHA256Context::update(const void *buffer, size_t len) noexcept {
    CryptHashData(this->hHash, (const BYTE *) buffer, len, 0);
}

void sese::security::evp::SHA256Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, (DWORD *) &length, 0);
}

void *sese::security::evp::SHA256Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::SHA256Context::getLength() noexcept {
    return 32;
}