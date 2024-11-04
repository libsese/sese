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

#include <sese/net/http/Cookie.h>
#include <sese/util/DateTime.h>

using namespace sese::net::http;

// GCOVR_EXCL_START

Cookie::Cookie(const std::string &name) noexcept {
    this->name = name;
}

// GCOVR_EXCL_STOP

Cookie::Cookie(const std::string &name, const std::string &value) noexcept {
    this->name = name;
    this->value = value;
}

bool Cookie::expired(const uint64_t NOW) const {
    return NOW > this->expires;
}

bool Cookie::isSecure() const {
    return secure;
}

void Cookie::setSecure(bool secure) {
    Cookie::secure = secure;
}

bool Cookie::isHttpOnly() const {
    return httpOnly;
}

void Cookie::setHttpOnly(bool http_only) {
    Cookie::httpOnly = http_only;
}

uint64_t Cookie::getMaxAge() const {
    return maxAge;
}

void Cookie::setMaxAge(uint64_t max_age) {
    Cookie::maxAge = max_age;
    // Cookie::expires = now + maxAge;
}

uint64_t Cookie::getExpires() const {
    return expires;
}

void Cookie::setExpires(uint64_t expires) {
    Cookie::expires = expires;
}

const std::string &Cookie::getName() const {
    return name;
}

const std::string &Cookie::getValue() const {
    return value;
}

void Cookie::setValue(const std::string &value) {
    Cookie::value = value;
}

const std::string &Cookie::getDomain() const {
    return domain;
}

void Cookie::setDomain(const std::string &domain) {
    Cookie::domain = domain;
}

const std::string &Cookie::getPath() const {
    return path;
}

void Cookie::setPath(const std::string &path) {
    Cookie::path = path;
}

void Cookie::updateExpiresFrom(uint64_t now) {
    if (Cookie::expires == 0 && Cookie::maxAge != 0) {
        Cookie::expires = now + Cookie::maxAge;
    }
}
