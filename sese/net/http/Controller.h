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
 * @brief Controller
 * @author kaoru
 * @version 0.1
 * @date September 13, 2023
 */

#pragma once

#include <sese/net/http/HttpServletContext.h>

#include <set>
#include <vector>
#include <functional>
#include <utility>

namespace sese::net::http {

/// HTTP applications
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
    /// Registration path
    std::string uri;
    /// Expected request type, 403 if not met
    RequestType expect_type;
    /// Expected query string parameters, 400 if not met
    std::set<std::string> expect_query_args;
    /// Expected header parameters, 400 if not met
    std::set<std::string> expect_headers;
    /// Request handling callback function, 500 if not met
    Callback callback;
};

/// HTTP controller
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

/// Define a controller
/// \param name Controller name
/// \param ... Controller members
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

/// Register a URL path
/// \param name Internal name
/// \param method Expected HTTP method
/// \param url Registered URL link
/// \note \<key\> Required parameter key is passed as an HTTP header,
/// {key} required parameter key is passed as a query parameter
#define SESE_URL(name, method, url)                                 \
    this->servlets.emplace_back(method, url);                       \
    sese::net::http::Servlet &name = servlets[servlets.size() - 1]; \
    name = [this](sese::net::http::HttpServletContext &ctx)
