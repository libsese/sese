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
 * @file Header.h
 * @brief HTTP Header Key-Value Collection
 * @author kaoru
 * @date May 17, 2022
 */

#pragma once

#include <sese/net/http/CookieMap.h>
#include <sese/container/StrCaseMap.h>
#include <sese/Util.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/// @brief HTTP Version
enum class HttpVersion {
    VERSION_1_1,
    VERSION_2,
    VERSION_UNKNOWN
};

/**
 * @brief HTTP Header Key-Value Collection
 */
class Header {
public:
    using Ptr = std::unique_ptr<Header>;
    using KeyValueType = std::pair<std::string, std::string>;

    explicit Header() = default;
    Header(const std::initializer_list<KeyValueType> &initializer_list) noexcept;
    virtual ~Header() = default;

    void set(const std::string &key, const std::string &value) noexcept;
    const std::string &get(const std::string &key, const std::string &default_value) noexcept;
#if SESE_CXX_STANDARD > 201700L
    std::string_view getView(const std::string &key, const std::string &default_value) noexcept;
#endif
    std::map<std::string, std::string>::iterator begin() noexcept { return headers.begin(); }
    std::map<std::string, std::string>::iterator end() noexcept { return headers.end(); }
    auto find(const std::string &key) noexcept { return headers.find(key); }

    void clear() { headers.clear(); }
    [[nodiscard]] bool empty() const { return headers.empty(); }
    [[nodiscard]] size_t size() const { return headers.size(); }

    /// Determine if a field exists
    /// @param key Header field name
    /// @return Result
    bool exist(const std::string &key) { return headers.find(key) != headers.end(); }
    /// Call this when certain the field exists
    /// @see sese::net::http::Header::exist
    /// @param key Header field name
    /// @return Value
    const std::string &get(const std::string &key) { return headers.at(key); }

    /// Get the current Cookie map
    /// \retval nullptr if the current map is empty
    [[nodiscard]] const CookieMap::Ptr &getCookies() const;
    /// Set the current Cookie map
    /// \param cookies Map to set
    void setCookies(const CookieMap::Ptr &cookies);

    /// Get a Cookie by name
    /// \param name Cookie name
    /// \retval nullptr if the Cookie does not exist
    [[nodiscard]] Cookie::Ptr getCookie(const std::string &name) const;
    /// Add a Cookie
    /// \param cookie Cookie to add
    void setCookie(const Cookie::Ptr &cookie);

protected:
    StrCaseMap<std::string> headers;
    CookieMap::Ptr cookies = nullptr;
};
} // namespace sese::net::http
