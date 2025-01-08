// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    CryptHashData(this->hHash, static_cast<const BYTE *>(buffer), static_cast<DWORD>(len), 0);
}

void sese::security::evp::MD5Context::final() noexcept {
    CryptGetHashParam(this->hHash, HP_HASHVAL, (BYTE *) this->result, reinterpret_cast<DWORD *>(&length), 0);
}

void *sese::security::evp::MD5Context::getResult() noexcept {
    return this->result;
}

size_t sese::security::evp::MD5Context::getLength() noexcept {
    return 16;
}
