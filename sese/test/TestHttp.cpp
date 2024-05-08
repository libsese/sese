#include <sese/net/Socket.h>
#include <sese/record/Marco.h>
#include <sese/net/http/HeaderBuilder.h>
#include <sese/net/http/RequestHeader.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/Range.h>
#include <sese/net/http/RequestParser.h>
#include <sese/io/InputBuffer.h>
#include <sese/io/OutputBuffer.h>

#include <gtest/gtest.h>

#define printf SESE_INFO

using namespace sese;

static auto makeRandomPortAddr() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", port);
    auto addr = sese::net::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}


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
    req.setUrl("/?key1=&%e4%bd%a0%e5%a5%bd233=value2");
    EXPECT_EQ(req.getQueryArg("key1", ""), "");
    EXPECT_EQ(req.getQueryArg("你好233", ""), "value2");
}

TEST(TestHttp, RequestHeader_args_error) {
    auto req = net::http::RequestHeader();
    EXPECT_NO_THROW(req.setUrl("/?&"));
    EXPECT_TRUE(req.queryArgsEmpty());
}

TEST(TestHttp, Header_misc) {
    using namespace sese::net::http;

    Header header;
    HeaderBuilder(header)
            .set("Key-A", "Value-A")
            .set("Key-B", "Value-B")
            .set("Key-C", "Value-C");

    for (auto &&item: header) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }

    EXPECT_TRUE(header.exist("Key-A"));
    EXPECT_FALSE(header.exist("Key-D"));
    EXPECT_EQ("Value-A", header.get("Key-A"));
    EXPECT_FALSE(header.empty());
    header.clear();
    EXPECT_TRUE(header.empty());
}
/// 非法的 HTTP 头部结尾
TEST(TestHttpUtil, GetLine_0) {
    auto str = "GET / HTTP/1.1";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 超过单行长度限制的 HTTP 头部
TEST(TestHttpUtil, GetLine_1) {
    char buffer[HTTP_MAX_SINGLE_LINE + 1]{};
    auto input = sese::io::InputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 非法的 FirstLine
TEST(TestHttpUtil, RecvRequest_0) {
    auto str = "GET / HTTP/1.1 Hello\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

/// 非法的 HTTP 版本
TEST(TestHttpUtil, RecvRequest_1) {
    auto str = "GET / HTTP/0.9\r\n\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    ASSERT_EQ(req.getVersion(), sese::net::http::HttpVersion::VERSION_UNKNOWN);
}

/// 接收字段失败
TEST(TestHttpUtil, RecvRequest_2) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvRequest(&input, &req));
}

TEST(TestHttpUtil, RecvRequest_3) {
    auto str = "GET / HTTP/1.1\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));
    ASSERT_EQ(req.getVersion(), sese::net::http::HttpVersion::VERSION_1_1);
}

TEST(TestHttpUtil, SendRequest_0) {
    char buffer[1024]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// 发送字段失败
TEST(TestHttpUtil, SendRequest_1) {
    char buffer[18]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::RequestHeader req{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_FALSE(sese::net::http::HttpUtil::sendRequest(&output, &req));
}

/// 非法的 HTTP 头部结尾
TEST(TestHttpUtil, RecvResponse_0) {
    auto str = "HTTP/1.1 200";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

/// 非法的 HTTP 版本
TEST(TestHttpUtil, RecvResponse_1) {
    auto str = "HTTP/0.1 200\r\n\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    ASSERT_EQ(resp.getVersion(), sese::net::http::HttpVersion::VERSION_UNKNOWN);
}

/// 接收字段错误
TEST(TestHttpUtil, RecvResponse_2) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    ASSERT_FALSE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
}

TEST(TestHttpUtil, RecvResponse_3) {
    auto str = "HTTP/1.1 200\r\n"
               "Version: 0.0.1\r\n"
               "\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));
    ASSERT_EQ(resp.getVersion(), sese::net::http::HttpVersion::VERSION_1_1);
}

/// 非法的 HTTP 版本
TEST(TestHttpUtil, SendResponse_0) {
    char buffer[1024]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{};
    resp.setVersion(sese::net::http::HttpVersion::VERSION_UNKNOWN);
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// 发送字段失败
TEST(TestHttpUtil, SendResponse_1) {
    char buffer[18]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

/// 发送版本失败
TEST(TestHttpUtil, SendResponse_2) {
    char buffer[5]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{};
    ASSERT_FALSE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpUtil, SendResponse_3) {
    char buffer[1024]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    sese::net::http::ResponseHeader resp{
            {"Host", "www.example.com"},
            {"Version", "0.0.1"}
    };
    ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpCookie, RecvRequestCookie) {
    auto str = "GET / HTTP/1.1\r\n"
               "Cookie: id=123; name=hello=xxx; user=foo\r\n"
               "\r\n";
    auto input = sese::io::InputBufferWrapper(str, strlen(str));
    sese::net::http::RequestHeader req;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvRequest(&input, &req));

    ASSERT_EQ(req.getCookies()->size(), 2);
    for (decltype(auto) cookie: *req.getCookies()) {
        SESE_INFO("cookie name: %s, value: %s", cookie.first.c_str(), cookie.second->getValue().c_str());
    }
}

TEST(TestHttpCookie, RecvResponseCookie) {
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
    auto input = sese::io::InputBufferWrapper(str, strlen(str));

    sese::net::http::ResponseHeader resp;
    ASSERT_TRUE(sese::net::http::HttpUtil::recvResponse(&input, &resp));

    ASSERT_EQ(resp.getCookies()->size(), 2);
    for (decltype(auto) cookie: *resp.getCookies()) {
        SESE_INFO("cookie name: %s, value: %s", cookie.first.c_str(), cookie.second->getValue().c_str());
    }
}

TEST(TestHttpCookie, SendRequestCookie) {
    char buffer[1024]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookie_map = std::make_shared<sese::net::http::CookieMap>();
    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookie_map->add(cookie);
    }

    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("token", "123456");
        cookie_map->add(cookie);
    }

    sese::net::http::RequestHeader resp{};
    resp.setCookies(cookie_map);

    ASSERT_TRUE(sese::net::http::HttpUtil::sendRequest(&output, &resp));
}

TEST(TestHttpCookie, SendResponseCookie) {
    char buffer[1024]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));

    auto cookie_map = std::make_shared<sese::net::http::CookieMap>();
    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("id", "foo");
        cookie->setValue("bar");
        cookie->setDomain("www.example.com");
        cookie->setHttpOnly(true);
        cookie->setMaxAge(10);
        cookie->setPath("/docs");
        cookie->setSecure(true);
        cookie_map->add(cookie);
    }

    {
        auto cookie = std::make_shared<sese::net::http::Cookie>("token", "123456");
        cookie->setExpires(1690155929);
        cookie_map->add(cookie);
    }

    sese::net::http::ResponseHeader resp;
    resp.setCookies(cookie_map);

    ASSERT_TRUE(sese::net::http::HttpUtil::sendResponse(&output, &resp));
}

TEST(TestHttpRange, Parse_0) {
    auto ranges = sese::net::http::Range::parse("bytes=200-1000, 2000-6576, 19000-", 20000);
    ASSERT_EQ(ranges.size(), 3);

    EXPECT_EQ(ranges[0].begin, 200);
    EXPECT_EQ(ranges[0].len, 801);


    EXPECT_EQ(ranges[1].begin, 2000);
    EXPECT_EQ(ranges[1].len, 4577);


    EXPECT_EQ(ranges[2].begin, 19000);
    EXPECT_EQ(ranges[2].len, 1000);
}

TEST(TestHttpRange, Parse_1) {
    auto ranges = sese::net::http::Range::parse("bytes=", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_2) {
    auto ranges = sese::net::http::Range::parse("block=1200-", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_3) {
    auto ranges = sese::net::http::Range::parse("bytes=1200-2000-3000", 20000);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_4) {
    auto ranges = sese::net::http::Range::parse("bytes=1200-3000", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_5) {
    auto ranges = sese::net::http::Range::parse("bytes=1-3000", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, Parse_6) {
    auto ranges = sese::net::http::Range::parse("bytes=99-1", 100);
    EXPECT_TRUE(ranges.empty());
}

TEST(TestHttpRange, toString) {
    auto ranges = sese::net::http::Range::parse("bytes=200-1000", 2000)[0];
    auto len = ranges.toStringLength(2000);
    EXPECT_EQ(len, ranges.toString(2000).length());
}

TEST(TestRequestParser, HostWithPort) {
    {
        auto result = sese::net::http::RequestParser::parse("https://127.0.0.1:7890/");
        ASSERT_NE(result.address, nullptr);
        EXPECT_EQ(result.address->getPort(), 7890);
    }
    {
        auto result = sese::net::http::RequestParser::parse("https://127.0.0.1:7890a/");
        ASSERT_EQ(result.address, nullptr);
    }
    {
        auto result = sese::net::http::RequestParser::parse("https://127.0.0.1:7890:5678/");
        ASSERT_EQ(result.address, nullptr);
    }
}