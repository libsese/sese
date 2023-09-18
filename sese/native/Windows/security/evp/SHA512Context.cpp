#include <sese/security/evp/SHA512Context.h>

sese::security::evp::SHA512Context::SHA512Context() noexcept : Context() {
    CryptAcquireContext(&this->hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(this->hProv, CALG_SHA_512, 0, 0, &this->hHash);
}

sese::security::evp::SHA512Context::~SHA512Context() noexcept {
    if (this->hProv) {
        CryptReleaseContext(this->hProv, 0);
    }

    if (this->hHash) {
        CryptDestroyHash(this->hHash);
    }
}

void sese::security::evp::SHA512Context::update(const void *buffer, size_t len) noexcept {
    CryptHashData(this->hHash, (const BYTE *) buffer, (DWORD) len, 0);
}

void sese::security::evp::SHA512Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, (DWORD *) &length, 0);
}

void *sese::security::evp::SHA512Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::SHA512Context::getLength() noexcept {
    return 64;
}