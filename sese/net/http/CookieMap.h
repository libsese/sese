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
 * \file CookieMap.h
 * \brief Cookie mapping collection class
 * \date December 20, 2022
 * \author kaoru
 * \version 0.1
 */

#pragma once

#include <sese/net/http/Cookie.h>
#include <map>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/// \brief Cookie mapping collection class
class  CookieMap {
public:
    using Ptr = std::shared_ptr<CookieMap>;
    using Map = std::map<std::string, Cookie::Ptr>;

    /// Add a cookie
    /// \param cookie Cookie to add
    void add(const Cookie::Ptr &cookie) noexcept;
    /// Retrieve an existing Cookie
    /// \param name Cookie name
    /// \retval nullptr If the specified Cookie does not exist
    Cookie::Ptr find(const std::string &name) noexcept;

    /// Check and remove expired cookies
    /// \param now Time in seconds
    void expired(uint64_t now);

    /// Convert max-age field in the collection to expires
    /// \param offset Time in seconds
    void updateExpiresFrom(uint64_t offset);

    [[nodiscard]] size_t size() const { return map.size(); }
    [[nodiscard]] bool empty() const { return map.empty(); }
    void clear() { map.clear(); }

    inline Map::iterator begin() noexcept { return map.begin(); }
    inline Map::iterator end() noexcept { return map.end(); }

private:
    Map map;
};
} // namespace sese::net::http