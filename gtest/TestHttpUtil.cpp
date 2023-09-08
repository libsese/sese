#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/Range.h>
#include <sese/util/InputBufferWrapper.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

/// 非法的 HTTP 头部结尾
GTEST_TEST(TestHttpUtil, GetLine_0) {
    auto str = "GET / HTTP/1.1";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 超过单行长度限制的 HTTP 头部
GTEST_TEST(TestHttpUtil, GetLine_1) {
    char buffer[HTTP_MAX_SINGLE_LINE + 1]{};
    auto input = sese::InputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 非法的 FirstLine
GTEST_TEST(TestHttpUtil, RecvRequest_0) {
    auto str = "GET / HTTP/1.1 Hello\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 非法的 HTTP 版本
GTEST_TEST(TestHttpUtil, RecvRequest_1) {
    auto str = "GET / HTTP/0.9\r\n\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    GTEST_ASSERT_EQ(req.getVersion(), sese::net::http::HttpVersion::VERSION_UNKNOWN);
}

/// 接收字段失败
GTEST_TEST(TestHttpUtil, RecvRequest_2) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

GTEST_TEST(TestHttpUtil, RecvRequest_3) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    GTEST_ASSERT_EQ(req.getVersion(), sese::net::http::HttpVersion::VERSION_1_1);
}

GTEST_TEST(TestHttpUtil, SendRequest_0) {
    char buffer[1024]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}};
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// 发送字段失败
GTEST_TEST(TestHttpUtil, SendRequest_1) {
    char buffer[18]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}};
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// 非法的 HTTP 头部结尾
GTEST_TEST(TestHttpUtil, RecvResponse_0) {
    auto str = "HTTP/1.1 200";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

/// 非法的 HTTP 版本
GTEST_TEST(TestHttpUtil, RecvResponse_1) {
    auto str = "HTTP/0.1 200\r\n\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    GTEST_ASSERT_EQ(resp.getVersion(), sese::net::http::HttpVersion::VERSION_UNKNOWN);
}

/// 接收字段错误
GTEST_TEST(TestHttpUtil, RecvResponse_2) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

GTEST_TEST(TestHttpUtil, RecvResponse_3) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    GTEST_ASSERT_EQ(resp.getVersion(), sese::net::http::HttpVersion::VERSION_1_1);
}

/// 非法的 HTTP 版本
GTEST_TEST(TestHttpUtil, SendResponse_0) {
    char buffer[1024]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{};
    resp.setVersion(sese::net::http::HttpVersion::VERSION_UNKNOWN);
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// 发送字段失败
GTEST_TEST(TestHttpUtil, SendResponse_1) {
    char buffer[18]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}};
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// 发送版本失败
GTEST_TEST(TestHttpUtil, SendResponse_2) {
    char buffer[5]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{};
    GTEST_ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

GTEST_TEST(TestHttpUtil, SendResponse_3) {
    char buffer[1024]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}};
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

GTEST_TEST(TestHttpCookie, RecvRequestCookie) {
    auto str = "GET / HTTP/1.1\r\n"
               "Cookie: id=123; name=hello=xxx; user=foo\r\n"
               "\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));
    sese::net::http::RequestHeader req;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));

    GTEST_ASSERT_EQ(req.getCookies()->size(), 2);
    for (decltype(auto) cookie: *req.getCookies()) {
        SESE_INFO("cookie name: %s, value: %s", cookie.first.c_str(), cookie.second->getValue().c_str());
    }
}

GTEST_TEST(TestHttpCookie, RecvResponseCookie) {
    auto str = "HTTP/1.1 200 OK\r\n"
               "Set-Cookie: "
               "id=bar; "
               "HttpOnly; "
               "Expires=Wed, 21 Oct 2015 07:28:00 GMT; "
               "Secure; "
               "Domain=www.example.com; "
               "Path=/\r\n"
               "Set-Cookie: "
               "name=foo; "
               "undef=undef; "
               "max-age=114514\r\n"
               "\r\n";
    auto input = sese::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));

    GTEST_ASSERT_EQ(resp.getCookies()->size(), 2);
    for (decltype(auto) cookie: *resp.getCookies()) {
        SESE_INFO("cookie name: %s, value: %s", cookie.first.c_str(), cookie.second->getValue().c_str());
    }
}

GTEST_TEST(TestHttpCookie, SendRequestCookie) {
    char buffer[1024]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookieMap = std::make_shared<sese::net::http::CookieMap>();
    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookieMap->add(cookie);
    }

    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("token", "123456");
        cookieMap->add(cookie);
    }

    sese::net::http::RequestHeader resp{};
    resp.setCookies(cookieMap);

    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &resp));
}

GTEST_TEST(TestHttpCookie, SendResponseCookie) {
    char buffer[1024]{};
    auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookieMap = std::make_shared<sese::net::http::CookieMap>();
    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookie->setDomain("www.example.com");
        cookie->setHttpOnly(true);
        cookie->setMaxAge(10);
        cookie->setPath("/docs");
        cookie->setSecure(true);
        cookieMap->add(cookie);
    }

    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("token", "123456");
        cookie->setExpires(1690155929);
        cookieMap->add(cookie);
    }

    sese::net::http::ResponseHeader resp;
    resp.setCookies(cookieMap);

    GTEST_ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

GTEST_TEST(TestHttpRange, Parse_0) {
    auto ranges = sese::net::http::Range::parse("bytes=200-1000, 2000-6576, 19000-", 20000);
    GTEST_ASSERT_EQ(ranges.size(), 3);

    EXPECT_EQ(ranges[0].begin, 200);
    EXPECT_EQ(ranges[0].len, 801);


    EXPECT_EQ(ranges[1].begin, 2000);
    EXPECT_EQ(ranges[1].len, 4577);


    EXPECT_EQ(ranges[2].begin, 19000);
    EXPECT_EQ(ranges[2].len, 1000);
}

GTEST_TEST(TestHttpRange, Parse_1) {
    auto ranges = sese::net::http::Range::parse("bytes=", 20000);
    GTEST_EXPECT_TRUE(ranges.empty());
}

GTEST_TEST(TestHttpRange, Parse_2) {
    auto ranges = sese::net::http::Range::parse("block=1200-", 20000);
    GTEST_EXPECT_TRUE(ranges.empty());
}

GTEST_TEST(TestHttpRange, Parse_3) {
    auto ranges = sese::net::http::Range::parse("bytes=1200-2000-3000", 20000);
    GTEST_EXPECT_TRUE(ranges.empty());
}

GTEST_TEST(TestHttpRange, Parse_4) {
    auto ranges = sese::net::http::Range::parse("bytes=1200-3000", 100);
    GTEST_EXPECT_TRUE(ranges.empty());
}

GTEST_TEST(TestHttpRange, Parse_5) {
    auto ranges = sese::net::http::Range::parse("bytes=1-3000", 100);
    GTEST_EXPECT_TRUE(ranges.empty());
}

GTEST_TEST(TestHttpRange, Parse_6) {
    auto ranges = sese::net::http::Range::parse("bytes=99-1", 100);
    GTEST_EXPECT_TRUE(ranges.empty());
}
