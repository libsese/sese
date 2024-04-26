/**
 * @file Controller.h
 * @brief 控制器
 * @author kaoru
 * @version 0.1
 * @date 2023年9月13日
 */

#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>

#include <functional>

namespace sese::net::http {

/// HTTP 控制器
class Controller {
public:
    using Callback = std::function<void(Request &req, Response &resp)>;

    Controller();

    Controller(const std::string &url, Callback callback);

    [[nodiscard]] const std::string &getUri() const { return uri; }

    void invoke(Request &req, Response &resp) const;
    void operator()(Request &req, Response &resp) const;

private:
    std::string uri;
    Callback callback;
};

} // namespace sese::net::http