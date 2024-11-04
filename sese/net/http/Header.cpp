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

#include <sese/net/http/Header.h>

#include <cctype>
// #include <algorithm>

using namespace sese::net::http;

Header::Header(const std::initializer_list<KeyValueType> &initializer_list) noexcept {
    for (const auto &item: initializer_list) {
        headers.insert(item);
    }
}

#if defined(SESE_PLATFORM_WINDOWS) && !defined(__MINGW32__)
#define XX std::tolower
#else
#define XX ::tolower
#endif

void Header::set(const std::string &key, const std::string &value) noexcept {
    // auto temp = key;
    // std::transform(temp.begin(), temp.end(), temp.begin(), XX);
    // headers[temp] = value;
    headers[key] = value;
}

const std::string &Header::get(const std::string &key, const std::string &default_value) noexcept {
    // auto temp = key;
    // std::transform(temp.begin(), temp.end(), temp.begin(), XX);
    // auto res = headers.find(temp);
    auto res = headers.find(key);
    if (res == headers.end()) {
        return default_value;
    } else {
        return res->second;
    }
}

#undef XX

#if SESE_CXX_STANDARD > 201700L

std::string_view Header::getView(const std::string &key, const std::string &default_value) noexcept {
    auto res = headers.find(key);
    if (res == headers.end()) {
        return default_value;
    } else {
        return res->second;
    }
}

#endif

const CookieMap::Ptr &Header::getCookies() const {
    return cookies;
}

void Header::setCookies(const CookieMap::Ptr &cookies) {
    Header::cookies = cookies;
}

// GCOVR_EXCL_START

Cookie::Ptr Header::getCookie(const std::string &name) const {
    if (Header::cookies) {
        return Header::cookies->find(name);
    }
    return nullptr;
}

void Header::setCookie(const Cookie::Ptr &cookie) {
    if (!Header::cookies) {
        Header::cookies = std::make_shared<CookieMap>();
    }
    Header::cookies->add(cookie);
}

// GCOVR_EXCL_STOP