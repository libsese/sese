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

#include "sese/net/IPAddress.h"

using namespace sese::net;

IPAddress::Ptr IPAddress::create(const char *address, uint16_t port) {
    addrinfo *res, hints{0};
    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    auto err = getaddrinfo(address, nullptr, &hints, &res);
    if (err) {
        return nullptr;
    }

    // GCOVR_EXCL_START
    IPAddress::Ptr result = std::dynamic_pointer_cast<IPAddress>(Address::create(res->ai_addr, static_cast<socklen_t>(res->ai_addrlen)));
    if (result) {
        result->setPort(port);
    }
    else {
        result = nullptr;
    }
    // GCOVR_EXCL_STOP

    freeaddrinfo(res);
    return result;
}
