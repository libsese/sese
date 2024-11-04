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

#include <sese/net/http/CookieMap.h>

using namespace sese::net::http;

void CookieMap::add(const Cookie::Ptr &cookie) noexcept {
    map[cookie->getName()] = cookie;
}

Cookie::Ptr CookieMap::find(const std::string &name) noexcept {
    auto iterator = map.find(name);
    if (iterator != map.end()) {
        return iterator->second;
    } else {
        return nullptr;
    }
}

// GCOVR_EXCL_START

void CookieMap::expired(uint64_t now) {
    for (auto iterator = map.begin(); iterator != map.end();) {
        if (iterator->second->getExpires()) {
            if (iterator->second->expired(now)) {
                map.erase(iterator++);
            } else {
                iterator++;
            }
        } else {
            iterator++;
        }
    }
}

void CookieMap::updateExpiresFrom(uint64_t offset) {
    for (auto iterator = map.begin(); iterator != map.end(); ++iterator) {
        iterator->second->updateExpiresFrom(offset);
    }
}

// GCOVR_EXCL_STOP