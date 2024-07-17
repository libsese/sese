#include <sese/security/evp/SHA384Context.h>

sese::security::evp::SHA384Context::SHA384Context() noexcept : Context() {
    CryptAcquireContext(&this->hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(this->hProv, CALG_SHA_384, 0, 0, &this->hHash);
}

sese::security::evp::SHA384Context::~SHA384Context() noexcept {
    if (this->hProv) {
        CryptReleaseContext(this->hProv, 0);
    }

    if (this->hHash) {
        CryptDestroyHash(this->hHash);
    }
}

void sese::security::evp::SHA384Context::update(const void *buffer, size_t len) noexcept {
    CryptHashData(this->hHash, static_cast<const BYTE *>(buffer), static_cast<DWORD>(len), 0);
}

void sese::security::evp::SHA384Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, reinterpret_cast<DWORD *>(&length), 0);
}

void *sese::security::evp::SHA384Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::SHA384Context::getLength() noexcept {
    return 48;
}