#include "sese/net/Socket.h"
#include "sese/record/Marco.h"
#include "sese/net/http/HeaderBuilder.h"
#include "sese/net/http/RequestHeader.h"

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

// 测试根目录转义
TEST(TestHttp, RequestHeader_root) {
    auto req = net::http::RequestHeader();
    req.setQueryArg("ssl", "enable");
    req.setQueryArg("user", "root");
    req.setQueryArg("pwd", "0x7c00");
    EXPECT_EQ(req.getUrl(), "/?pwd=0x7c00&ssl=enable&user=root");
}

// 测试多级目录转义
TEST(TestHttp, RequestHeader_dirs) {
    auto req = net::http::RequestHeader();
    req.setUri("/你好233/index");
    EXPECT_EQ(req.getUrl(), "/%E4%BD%A0%E5%A5%BD233/index");
}

// 查询参数转义
TEST(TestHttp, RequestHeader_args) {
    auto req = net::http::RequestHeader();
    req.setUrl("/?key1=&%E4%BD%A0%E5%A5%BD233=value2");
    EXPECT_EQ(req.getQueryArg("key1", ""), "");
    EXPECT_EQ(req.getQueryArg("你好233", ""), "value2");
}

TEST(TestHttp, RequestHeader_args_error) {
    auto req = net::http::RequestHeader();
    EXPECT_NO_THROW(req.setUrl("/?&"));
    EXPECT_TRUE(req.queryArgsEmpty());
}

TEST(TestHttp, HeaderBuilder) {
    using namespace sese::net::http;

    Header header;
    HeaderBuilder(header)
            .set("Key-A", "Value-A")
            .set("Key-B", "Value-B")
            .set("Key-C", "Value-C");

    for (auto &&item: header) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
}