#include <sese/net/http/Controller.h>
#include "sese/net/http/HttpUtil.h"

sese::net::http::Servlet::Servlet() {
    expect_type = RequestType::GET;
    uri = "/";
}

sese::net::http::Servlet::Servlet(RequestType expect, const std::string &url) {
    expect_type = expect;
    auto pos = url.find('?');
    if (pos == std::string::npos) {
        uri = url;
    } else {
        uri = url.substr(0, pos);
    }
}

void sese::net::http::Servlet::setCallback(const Callback &callback) {
    this->callback = callback;
}

void sese::net::http::Servlet::requiredQueryArg(const std::string &arg) {
    this->expect_query_args.emplace(arg);
}

void sese::net::http::Servlet::requiredHeader(const std::string &arg) {
    this->expect_headers.emplace(arg);
}

void sese::net::http::Servlet::invoke(Request &req, Response &resp) const {
    if (!callback) {
        resp.setCode(500);
        return;
    }

    if (req.getType() != expect_type) {
        resp.setCode(403);
        return;
    }

    for (auto &&item: expect_headers) {
        if (!req.exist(item)) {
            resp.setCode(400);
            return;
        }
    }

    for (auto &&item: expect_query_args) {
        if (!req.queryArgsExist(item)) {
            resp.setCode(400);
            return;
        }
    }

    callback(req, resp);
}
