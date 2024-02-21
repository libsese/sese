#include <sese/security/SSLContextBuilder.h>
#include <sese/net/http/HttpClient.h>
#include <sese/record/Marco.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

using namespace std::chrono_literals;

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d\n", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

// 重定向
void redirect(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) noexcept {
    SESE_INFO("req GET: %s", req.getUrl().c_str());
    resp.setCode(301);
    resp.set("Location", "/LICENSE");
}

// post 测试
void post(
        sese::net::http::Request &req,
        sese::net::http::Response &resp
) noexcept {
    char *end;
    auto len = std::strtol(req.get("Content-Length", "0").c_str(), &end, 0);
    if (len != 0) {
        char b[1024]{};
        req.getBody().read(b, len);
        SESE_INFO("req POST: %s\n%s", req.getUrl().c_str(), b);
    } else {
        SESE_INFO("req POST: %s", req.getUrl().c_str());
    }

    std::string content = "Hello Client";
    resp.setCode(200);
    resp.getBody().write(content.c_str(), content.length());
}