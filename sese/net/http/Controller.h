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
 * @file Controller.h
 * @brief 控制器
 * @author kaoru
 * @version 0.1
 * @date 2023年9月13日
 */

#pragma once

#include <sese/net/http/HttpServletContext.h>

#include <set>
#include <vector>
#include <functional>
#include <utility>

namespace sese::net::http {

/// HTTP 应用
class Servlet {
public:
    using Ptr = std::shared_ptr<Servlet>;
    using Callback = std::function<void(HttpServletContext &)>;

    Servlet();

    Servlet(RequestType expect, const std::string &url);

    void setCallback(Callback callback);

    [[nodiscard]] const std::string &getUri() const { return uri; }

    [[nodiscard]] RequestType getExpectType() const { return expect_type; }

    [[nodiscard]] auto &getExpectQueryArgs() const { return expect_query_args; }

    [[nodiscard]] auto &getExpectHeaders() const { return expect_headers; }

    void requiredQueryArg(const std::string &arg);

    void requiredHeader(const std::string &arg);

    void invoke(HttpServletContext &ctx) const;

    Servlet &operator=(Callback callback) {
        setCallback(std::move(callback));
        return *this;
    }

private:
    /// 注册路径
    std::string uri;
    /// 预期的请求类型，不满足则 403
    RequestType expect_type;
    /// 预期的查询字符串参数，不满足则 400
    std::set<std::string> expect_query_args;
    /// 预期的头部参数，不满足则 400
    std::set<std::string> expect_headers;
    /// 请求处理回调函数，不满足则 500
    Callback callback;
};

/// HTTP 控制器
class Controller {
public:
    using Ptr = std::shared_ptr<Controller>;

    Controller() = default;
    virtual ~Controller() = default;

    virtual void init() = 0;

    auto begin() { return servlets.begin(); }
    auto end() { return servlets.end(); }

protected:
    std::vector<Servlet> servlets;
};

} // namespace sese::net::http

/// 定义一个控制器
/// @param name 控制器名称
/// @param ... 控制器成员
#define SESE_CTRL(name, ...)                                \
    class name final : public sese::net::http::Controller { \
    public:                                                 \
        using RequestType = sese::net::http::RequestType;   \
        name() : Controller() {                             \
        }                                                   \
        void init() override;                               \
                                                            \
    private:                                                \
        __VA_ARGS__;                                        \
    };                                                      \
    void name::init()

/// 注册一个 URL 路径
/// @param name 内部名称
/// @param method 期望的 http 方法
/// @param url 注册 URL 链接
/// @note \<key\> 需求参数 key 作为 http header 传递， {key} 需求参数 key 作为 query 参数传递
#define SESE_URL(name, method, url)                                 \
    this->servlets.emplace_back(method, url);                       \
    sese::net::http::Servlet &name = servlets[servlets.size() - 1]; \
    name = [this](sese::net::http::HttpServletContext &ctx)
