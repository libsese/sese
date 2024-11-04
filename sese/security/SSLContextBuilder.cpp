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

#include <openssl/ssl.h>

#include <sese/security/SSLContextBuilder.h>

using namespace sese::security;

SSLContext::Ptr SSLContextBuilder::SSL4Client() noexcept {
    // auto *method = SSLv23_client_method();
    auto *method = TLS_client_method();
    return std::make_shared<SSLContext>(method);
}

SSLContext::Ptr SSLContextBuilder::SSL4Server() noexcept {
    // auto method = SSLv23_server_method();
    auto method = TLS_server_method();
    return std::make_shared<SSLContext>(method);
}

std::unique_ptr<SSLContext> SSLContextBuilder::UniqueSSL4Client() noexcept {
    auto *method = TLS_client_method();
    return std::make_unique<SSLContext>(method);
}

std::unique_ptr<SSLContext> SSLContextBuilder::UniqueSSL4Server() noexcept {
    auto *method = TLS_server_method();
    return std::make_unique<SSLContext>(method);
}
