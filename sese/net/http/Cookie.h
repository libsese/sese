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
 * @file Cookie.h
 * @author kaoru
 * @brief Cookie class
 * @date September 13, 2023
 * @version 0.1
 */

#pragma once

#include <sese/Config.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/// Cookie class
class  Cookie {
public:
    using Ptr = std::shared_ptr<Cookie>;

    explicit Cookie(const std::string &name) noexcept;
    Cookie(const std::string &name, const std::string &value) noexcept;

public:
    [[nodiscard]] bool expired(uint64_t now) const;

    [[nodiscard]] bool isSecure() const;
    void setSecure(bool secure);
    [[nodiscard]] bool isHttpOnly() const;
    void setHttpOnly(bool http_only);
    [[nodiscard]] uint64_t getMaxAge() const;
    void setMaxAge(uint64_t max_age);
    [[nodiscard]] uint64_t getExpires() const;
    void setExpires(uint64_t expires);
    [[nodiscard]] const std::string &getName() const;
    [[nodiscard]] const std::string &getValue() const;
    void setValue(const std::string &value);
    [[nodiscard]] const std::string &getDomain() const;
    void setDomain(const std::string &domain);
    [[nodiscard]] const std::string &getPath() const;
    void setPath(const std::string &path);

    void updateExpiresFrom(uint64_t now);

private:
    bool secure = false;
    bool httpOnly = false;

    uint64_t maxAge = 0;
    uint64_t expires = 0;

    std::string name;
    std::string value;
    std::string domain;
    std::string path;
};
} // namespace sese::net::http