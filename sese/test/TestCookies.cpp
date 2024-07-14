#include <sese/net/http/HttpUtil.h>
#include <sese/util/DateTime.h>

#include <gtest/gtest.h>

TEST(TestCookies, ParseSetCookie_0) {
    auto str = "name=value; Domain=domain-value; Secure; HttpOnly; Path=/www; Expires=Sun, 20 Jun 2021 08:46:48 GMT";
    auto cookie = sese::net::http::HttpUtil::parseFromSetCookie(str);
    EXPECT_EQ("name", cookie->getName());
    EXPECT_EQ("value", cookie->getValue());
    EXPECT_EQ("domain-value", cookie->getDomain());
    EXPECT_EQ("/www", cookie->getPath());
    EXPECT_EQ(1624178808, cookie->getExpires());
    EXPECT_TRUE(cookie->isSecure());
    EXPECT_TRUE(cookie->isHttpOnly());
}

TEST(TestCookies, ParseSetCookie_1) {
    auto str = "name=value; Domain=domain-value; Secure; HttpOnly; Path=/www; Max-Age=101";
    auto cookie = sese::net::http::HttpUtil::parseFromSetCookie(str);
    EXPECT_EQ(101, cookie->getMaxAge());
}

TEST(TestCookies, ParseCookie) {
    auto str = "cookie1=value1; cookie2=value2; cookie3=value3";
    auto cookie = sese::net::http::HttpUtil::parseFromCookie(str);
    EXPECT_EQ(3, cookie->size());

    EXPECT_EQ("value1", cookie->find("cookie1")->getValue());
    EXPECT_EQ("value2", cookie->find("cookie2")->getValue());
    EXPECT_EQ("value3", cookie->find("cookie3")->getValue());

    EXPECT_EQ(nullptr, cookie->find("cookie4"));
}

TEST(TestCookies, Expired) {
    auto now = sese::DateTime::now(0);
    {
        auto str = "AAA=your_value; Expires=Fri, 12 Jul 2024 10:00:00 GMT; Path=/; Secure; HttpOnly";
        auto cookie = sese::net::http::HttpUtil::parseFromSetCookie(str);
        EXPECT_NE(cookie, nullptr);
        ASSERT_EQ(cookie->getMaxAge(), 0);
        EXPECT_TRUE(cookie->expired(now.getTimestamp()));
    }
    {
        auto str = "Set-Cookie: AAA=your_value; Max-Age=10; Path=/; Secure; HttpOnly";
        auto cookie = sese::net::http::HttpUtil::parseFromSetCookie(str);
        EXPECT_NE(cookie, nullptr);
        ASSERT_GT(cookie->getMaxAge(), 0);
        cookie->updateExpiresFrom(now.getTimestamp());
        EXPECT_TRUE(cookie->expired(now.getTimestamp() + 20));
    }
}