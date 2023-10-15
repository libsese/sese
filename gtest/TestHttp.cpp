#include "sese/net/Socket.h"
#include "sese/record/Marco.h"

#include "gtest/gtest.h"

#include <chrono>

#define printf SESE_INFO

using namespace sese;

static auto makeRandomPortAddr() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", port);
    auto addr = sese::net::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

#include <sese/net/http/UrlHelper.h>
#include <sese/net/http/QueryString.h>

TEST(TestHttp, UrlHelper_0) {
    sese::net::http::Url info("file:///C:/vcpkg/vcpkg.exe?ssl=enable&token=123456&");
    EXPECT_EQ(info.getProtocol(), "file");
    EXPECT_EQ(info.getHost(), "");
    EXPECT_EQ(info.getUrl(), "/C:/vcpkg/vcpkg.exe");
    EXPECT_EQ(info.getQuery(), "?ssl=enable&token=123456&");
}

TEST(TestHttp, UrlHelper_1) {
    sese::net::http::Url info("https://www.example.com/index.html?");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "www.example.com");
    EXPECT_EQ(info.getUrl(), "/index.html");
    EXPECT_EQ(info.getQuery(), "?");
}

TEST(TestHttp, UrlHelper_2) {
    sese::net::http::Url info("https://www.example.com/index.html");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "www.example.com");
    EXPECT_EQ(info.getUrl(), "/index.html");
    EXPECT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_3) {
    sese::net::http::Url info("https://localhost:8080");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "localhost:8080");
    EXPECT_EQ(info.getUrl(), "/");
    EXPECT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_4) {
    sese::net::http::Url info("https://localhost:8080?a=b");
    EXPECT_EQ(info.getProtocol(), "https");
    EXPECT_EQ(info.getHost(), "localhost:8080");
    EXPECT_EQ(info.getUrl(), "/");
    EXPECT_EQ(info.getQuery(), "?a=b");
}

TEST(TestHttp, QueryString_toString_0) {
    sese::net::http::QueryString q;
    q.set("ssl", "enable");
    q.set("user", "root");
    q.set("pwd", "0x7c00");
    ASSERT_EQ(q.toString(), "?pwd=0x7c00&ssl=enable&user=root");
}

TEST(TestHttp, QueryString_toString_1) {
    sese::net::http::QueryString q;
    ASSERT_TRUE(q.toString().empty());
}

TEST(TestHttp, QueryString_0) {
    sese::net::http::QueryString q("?ssl=enable&token=123456&uni=ABC%e4%bd%a0%e5%a5%bd%26%3d%2d&");
    auto enable = q.get("ssl", "disable");
    auto token = q.get("token", "undef");
    auto mode = q.get("mode", "undef");
    auto uni = q.get("uni", "undef");
    ASSERT_EQ(enable, "enable");
    ASSERT_EQ(token, "123456");
    ASSERT_EQ(mode, "undef");
    ASSERT_EQ(uni, "ABC你好&=-");
    ASSERT_EQ(q.size(), 3);

    for (decltype(auto) item: q) {
        SESE_INFO("%s:%s", item.first.c_str(), item.second.c_str());
    }
}

TEST(TestHttp, QueryString_1) {
    sese::net::http::QueryString q("?");
    ASSERT_TRUE(q.empty());
}

TEST(TestHttp, QueryString_2) {
    sese::net::http::QueryString q("?key=");
    ASSERT_TRUE(q.empty());
}

TEST(TestHttp, QueryString_3) {
    sese::net::http::QueryString q("?key1=&key2=value2");
    ASSERT_EQ(q.size(), 1);
}

TEST(TestHttp, QueryString_4) {
    sese::net::http::QueryString q("?&");
    ASSERT_TRUE(q.empty());
}