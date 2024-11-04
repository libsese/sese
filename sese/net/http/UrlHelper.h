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
 * @file UrlHelper.h
 * @date 2022年6月6日
 * @author kaoru
 * @brief Url 解析器
 */

#pragma once

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

#include <sese/Config.h>

namespace sese::net::http {

/// Url 解析器
class  Url final {
public:
    Url() = default;

    explicit Url(const std::string &url) noexcept;

    [[nodiscard]] const std::string &getProtocol() const {
        return protocol;
    }

    [[nodiscard]] const std::string &getHost() const {
        return host;
    }

    [[nodiscard]] const std::string &getUrl() const {
        return url;
    }

    [[nodiscard]] const std::string &getQuery() const {
        return query;
    }

private:
    /// 协议
    std::string protocol;
    /// 域名
    std::string host;
    /// 资源
    std::string url;
    /// 查询
    std::string query;
};
} // namespace sese::net::http