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

#include <sese/security/SecurityConfig.h>

int32_t sese::security::SecurityInitTask::init() noexcept {
    SSL_library_init();
    SSL_load_error_strings();
    return 0;
}

int32_t sese::security::SecurityInitTask::destroy() noexcept {
    OPENSSL_thread_stop();
    OPENSSL_cleanup();
    return 0;
}