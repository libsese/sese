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

#include <openssl/evp.h>

sese::security::evp::MD5Context::MD5Context() noexcept : Context() {
    this->context = EVP_MD_CTX_new();
    EVP_DigestInit((EVP_MD_CTX *) this->context, EVP_md5());
}

sese::security::evp::MD5Context::~MD5Context() noexcept {
    EVP_MD_CTX_free((EVP_MD_CTX *) this->context);
}

void sese::security::evp::MD5Context::update(const void *buffer, size_t len) noexcept {
    EVP_DigestUpdate((EVP_MD_CTX *) this->context, buffer, len);
}

void sese::security::evp::MD5Context::final() noexcept {
    EVP_DigestFinal((EVP_MD_CTX *) this->context, result, nullptr);
}

void *sese::security::evp::MD5Context::getResult() noexcept {
    return result;
}

size_t sese::security::evp::MD5Context::getLength() noexcept {
    return length;
}