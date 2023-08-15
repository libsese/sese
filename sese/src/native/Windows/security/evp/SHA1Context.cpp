#include <sese/security/evp/SHA1Context.h>

sese::security::evp::SHA1Context::SHA1Context() noexcept : Context() {
    CryptAcquireContext(&this->hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(this->hProv, CALG_SHA1, 0, 0, &this->hHash);
}

sese::security::evp::SHA1Context::~SHA1Context() noexcept {
    if (this->hProv) {
        CryptReleaseContext(this->hProv, 0);
    }

    if (this->hHash) {
        CryptDestroyHash(this->hHash);
    }
}

void sese::security::evp::SHA1Context::update(const void *buffer, size_t len) noexcept {
    CryptHashData(this->hHash, (const BYTE *) buffer, len, 0);
}

void sese::security::evp::SHA1Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, (DWORD *) &length, 0);
}

void *sese::security::evp::SHA1Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::SHA1Context::getLength() noexcept {
    return 20;
}
