#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif _WIN32
#pragma warning(disable : 4996)
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/Socket.h"
#include "sese/net/http/V2HttpServer.h"
#include "sese/util/Random.h"
#include "gtest/gtest.h"

#include <chrono>

using namespace sese;

auto makeRandomPortAddr() {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024) + 1024);
    printf("select port %d\n", port);
    auto addr = sese::net::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

TEST(TestHttp, DISABLED_V2HttpServer) {
    auto addr = makeRandomPortAddr();
    ASSERT_TRUE(addr != nullptr);

    net::v2::http::HttpServer serv;
    serv.setBindAddress(addr);
    serv.setKeepAlive(10);
    ASSERT_TRUE(serv.init());
    serv.start();

    char buf0[]{"GET / HTTP/1.1\r\n"
                "connect: keep-alive\r\n\r\n"};
    char buf1[256]{};
    int64_t writeSize;

    sese::net::Socket client(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
    if (client.connect(addr)) {
        EXPECT_TRUE(false);
        goto shutdown;
    }

    for (int i = 0; i < 5; ++i) {
        writeSize = client.write(buf0, sizeof(buf0) - 1);
        if (writeSize <= 0) {
            EXPECT_TRUE(false);
            goto close;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        client.read(buf1, sizeof(buf1));
        printf("%s\n", buf1);
        memset(buf1, 0, sizeof(buf1));
    }

close:
    client.shutdown(sese::net::Socket::ShutdownMode::Both);
    client.close();
    std::this_thread::sleep_for(std::chrono::seconds(1));
shutdown:
    serv.shutdown();
}

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif

#include <sese/record/Marco.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/http/QueryString.h>

TEST(TestHttp, UrlHelper_0) {
    sese::net::http::Url info("file:///C:/vcpkg/vcpkg.exe?ssl=enable&token=123456&");
    ASSERT_EQ(info.getProtocol(), "file");
    ASSERT_EQ(info.getHost(), "");
    ASSERT_EQ(info.getUrl(), "/C:/vcpkg/vcpkg.exe");
    ASSERT_EQ(info.getQuery(), "?ssl=enable&token=123456&");
}

TEST(TestHttp, UrlHelper_1) {
    sese::net::http::Url info("https://www.example.com/index.html?");
    ASSERT_EQ(info.getProtocol(), "https");
    ASSERT_EQ(info.getHost(), "www.example.com");
    ASSERT_EQ(info.getUrl(), "/index.html");
    ASSERT_EQ(info.getQuery(), "?");
}

TEST(TestHttp, UrlHelper_2) {
    sese::net::http::Url info("https://www.example.com/index.html");
    ASSERT_EQ(info.getProtocol(), "https");
    ASSERT_EQ(info.getHost(), "www.example.com");
    ASSERT_EQ(info.getUrl(), "/index.html");
    ASSERT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_3) {
    sese::net::http::Url info("/index.html");
    ASSERT_EQ(info.getProtocol(), "");
    ASSERT_EQ(info.getHost(), "");
    ASSERT_EQ(info.getUrl(), "/index.html");
    ASSERT_EQ(info.getQuery(), "");
}

TEST(TestHttp, UrlHelper_4) {
    sese::net::http::Url info("index.html");
    ASSERT_EQ(info.getProtocol(), "");
    ASSERT_EQ(info.getHost(), "");
    ASSERT_EQ(info.getUrl(), "index.html");
    ASSERT_EQ(info.getQuery(), "");
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
    sese::net::http::QueryString q("?ssl=enable&token=123456&");
    auto enable = q.get("ssl", "disable");
    auto token = q.get("token", "undef");
    auto mode = q.get("mode", "undef");
    ASSERT_EQ(enable, "enable");
    ASSERT_EQ(token, "123456");
    ASSERT_EQ(mode, "undef");
    ASSERT_EQ(q.size(), 2);

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