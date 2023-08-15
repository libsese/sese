#include <sese/security/evp/MD5Context.h>

sese::security::evp::MD5Context::MD5Context() noexcept : Context() {
    CryptAcquireContext(&this->hProv, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(this->hProv, CALG_MD5, 0, 0, &this->hHash);
}

sese::security::evp::MD5Context::~MD5Context() noexcept {
    if (this->hProv) {
        CryptReleaseContext(this->hProv, 0);
    }

    if (this->hHash) {
        CryptDestroyHash(this->hHash);
    }
}

void sese::security::evp::MD5Context::update(const void *buffer, size_t len) noexcept {
    CryptHashData(this->hHash, (const BYTE *) buffer, len, 0);
}

void sese::security::evp::MD5Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, (DWORD *) &length, 0);
}

void *sese::security::evp::MD5Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::MD5Context::getLength() noexcept {
    return 16;
}
