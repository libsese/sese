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
 * @author kaoru
 * @brief HTTP 头部键值集合
 * @date 2022年05月17日
 */

#pragma once

#include <sese/net/http/CookieMap.h>
#include <sese/container/StrCaseMap.h>
#include <sese/Util.h>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::net::http {

/// @brief HTTP 版本
enum class HttpVersion {
    VERSION_1_1,
    VERSION_2,
    VERSION_UNKNOWN
};

/**
 * @brief HTTP 头部键值集合
 */
class  Header {
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

    /// 判断某个字段是否存在
    /// @param key 头部字段名称
    /// @return 结果
    bool exist(const std::string &key) { return headers.find(key) != headers.end(); }
    /// 当确定一定存在此字段时可以调用
    /// @see sese::net::http::Header::exist
    /// @param key 头部字段名称
    /// @return 值
    const std::string &get(const std::string &key) { return headers.at(key); }

    /// 获取当前 Cookie 映射集
    /// \retval nullptr 当前映射集为空
    [[nodiscard]] const CookieMap::Ptr &getCookies() const;
    /// 设置当前 Cookie 映射集
    /// \param cookies 欲设置的映射集
    void setCookies(const CookieMap::Ptr &cookies);

    /// 通过名称获取 Cookie
    /// \param name Cookie 名称
    /// \retval nullptr Cookie 不存在
    [[nodiscard]] Cookie::Ptr getCookie(const std::string &name) const;
    /// 添加一个 Cookie
    /// \param cookie 欲添加的 Cookie
    void setCookie(const Cookie::Ptr &cookie);

protected:
    StrCaseMap<std::string> headers;
    CookieMap::Ptr cookies = nullptr;
};
} // namespace sese::net::http