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

/// \file SM3Context.h
/// \brief EVP SM3 algorithm context
/// \author kaoru
/// \date April 11, 2024

#pragma once

#include <sese/security/evp/Context.h>

namespace sese::security::evp {

/// EVP SM3 algorithm context
class SM3Context : public Context {
public:
    SM3Context() noexcept;
    ~SM3Context() noexcept override;
    void update(const void *buffer, size_t len) noexcept override;
    void final() noexcept override;
    void *getResult() noexcept override;
    size_t getLength() noexcept override;

private:
    size_t length = 32;
    uint8_t result[32]{};
    void *context = nullptr;
};

}