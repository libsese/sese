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

/// \file WebsocketAuthenticator.h
/// \brief Websocket Authentication Suite
/// \author kaoru
/// \version 0.1
/// \date August 10, 2023

#pragma once

#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"
#include <utility>

namespace sese::net::ws {

/// Websocket Authenticator
class  WebsocketAuthenticator final : public NotInstantiable {
public:
    WebsocketAuthenticator() = delete;

    /// Generate key pair
    /// \return Key pair
    static std::pair<std::unique_ptr<char[]>, std::unique_ptr<char[]>> generateKeyPair() noexcept;

    /// Validate key pair
    /// \param key Key
    /// \param result Result
    /// \return Validation result
    static bool verify(const char *key, const char *result) noexcept;

    /// Generate result from key
    /// \param key Key
    /// \return Result
    static std::unique_ptr<char[]> toResult(const char *key) noexcept;

private:
    static const char *append_string;

    /// Generate a binary key
    static void generateKey(uint8_t key[16]);
};
} // namespace sese::net::ws