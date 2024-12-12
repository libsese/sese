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

/**
 * @file Address.h
 * @author kaoru
 * @date April 4, 2022
 * @brief Address class
 */

#pragma once

#ifdef _WIN32
#ifdef __MINGW32__
#define _WIN32_WINNET 0x602
#endif
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "sese/Config.h"

#include <memory>
#include <string>
#include <vector>


namespace sese::net {

template<typename T>
static T CreateMask(uint32_t bits) {
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}

/**
 * @brief Address class
 */
class  Address {
public:
    using Ptr = std::shared_ptr<Address>;

public:
    static Ptr create(const sockaddr *address, socklen_t address_len);

    static bool lookUp(std::vector<Address::Ptr> &result, const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

    static Address::Ptr lookUpAny(const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

public:
    virtual ~Address() = default;
    [[nodiscard]] virtual sockaddr *getRawAddress() const noexcept = 0;
    [[nodiscard]] virtual socklen_t getRawAddressLength() const noexcept = 0;
    [[nodiscard]] virtual std::string getAddress() const noexcept = 0;
};

extern "C" int inetPton(int af, const char *src, void *dst) noexcept;
} // namespace sese::net