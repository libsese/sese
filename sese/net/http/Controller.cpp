#include <sese/net/http/Controller.h>
#include <sese/text/StringBuilder.h>

// GCOVR_EXCL_START

sese::net::http::Servlet::Servlet() {
    expect_type = RequestType::GET;
    uri = "/";
}

// GCOVR_EXCL_STOP

sese::net::http::Servlet::Servlet(RequestType expect, const std::string &url) {
    using text::StringBuilder;
    expect_type = expect;
    auto pos = url.find('?');
    if (pos == std::string::npos) {
        uri = url;
        return;
    }
    uri = url.substr(0, pos);
    auto args_stirng = url.substr(pos + 1, url.length() - pos);
    auto args = StringBuilder::split(args_stirng, "&");
    for (auto &&item: args) {
        if (item.empty()) { continue; }
        if (StringBuilder::startsWith(item, "{") && StringBuilder::endsWith(item, "}")) {
            auto key = item.substr(1, item.length() - 2);
            if (key.empty()) { continue; }
            expect_query_args.emplace(key);
        } else if (StringBuilder::startsWith(item, "<") && StringBuilder::endsWith(item, ">")) {
            auto key = item.substr(1, item.length() - 2);
            if (key.empty()) { continue; }
            expect_headers.emplace(key);
        }
    }
}

// GCOVR_EXCL_START

void sese::net::http::Servlet::setCallback(Callback callback) { this->callback = std::move(callback); }

void sese::net::http::Servlet::requiredQueryArg(const std::string &arg) { this->expect_query_args.emplace(arg); }

void sese::net::http::Servlet::requiredHeader(const std::string &arg) { this->expect_headers.emplace(arg); }

// GCOVR_EXCL_STOP

void sese::net::http::Servlet::invoke(HttpServletContext &ctx) const {
    auto &&req = ctx.getReq();
    auto &&resp = ctx.getResp();
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

    callback(ctx);
}