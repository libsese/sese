#include <sese/net/dns/DnsServer.h>
#include <sese/net/dns/DNSUtil.h>
#include <sese/record/Marco.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

using namespace std::chrono_literals;

TEST(TestDNS, Decode_0) {
    const uint8_t buffer[12] = {0xc1, 0xa8, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    sese::net::dns::FrameHeaderInfo info{};
    sese::net::dns::DNSUtil::decodeFrameHeaderInfo(buffer, info);

    EXPECT_EQ(info.transactionId, 0xc1a8);

    EXPECT_EQ(info.flags.QR, 0);
    EXPECT_EQ(info.flags.opcode, 0);
    EXPECT_EQ(info.flags.TC, 0);
    EXPECT_EQ(info.flags.RD, 1);
    EXPECT_EQ(info.flags.z, 0);

    EXPECT_EQ(info.questions, 1);
    EXPECT_EQ(info.answerPrs, 0);
    EXPECT_EQ(info.authorityPrs, 0);
    EXPECT_EQ(info.additionalPrs, 0);
}

TEST(TestDNS, Decode_1) {
    const uint8_t buffer[12] = {0xc1, 0xa8, 0x81, 0x83, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};

    sese::net::dns::FrameHeaderInfo info{};
    sese::net::dns::DNSUtil::decodeFrameHeaderInfo(buffer, info);

    EXPECT_EQ(info.transactionId, 0xc1a8);

    EXPECT_EQ(info.flags.QR, 1);
    EXPECT_EQ(info.flags.opcode, 0);
    EXPECT_EQ(info.flags.TC, 0);
    EXPECT_EQ(info.flags.RD, 1);
    EXPECT_EQ(info.flags.RA, 1);
    EXPECT_EQ(info.flags.z, 0);
    EXPECT_EQ(info.flags.rcode, SESE_DNS_RCODE_NAME_ERROR);

    EXPECT_EQ(info.questions, 1);
    EXPECT_EQ(info.answerPrs, 0);
    EXPECT_EQ(info.authorityPrs, 1);
    EXPECT_EQ(info.additionalPrs, 0);
}

TEST(TestDNS, Encode_0) {
    const uint8_t expect[12] = {0x82, 0x67, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t result[12]{};

    sese::net::dns::FrameHeaderInfo info{};
    info.transactionId = 0x8267;

    info.flags.QR = 0;
    info.flags.opcode = 0;
    info.flags.TC = 0;
    info.flags.RD = 1;
    info.flags.z = 0;

    info.questions = 1;
    info.answerPrs = 0;
    info.authorityPrs = 0;
    info.additionalPrs = 0;

    sese::net::dns::DNSUtil::encodeFrameHeaderInfo(result, info);
    EXPECT_EQ(memcmp(expect, result, 12), 0);
}

TEST(TestDNS, Encode_1) {
    const uint8_t expect[12] = {0x82, 0x67, 0x81, 0x83, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    uint8_t result[12]{};

    sese::net::dns::FrameHeaderInfo info{};
    info.transactionId = 0x8267;

    info.flags.QR = 1;
    info.flags.opcode = 0;
    info.flags.TC = 0;
    info.flags.RD = 1;
    info.flags.RA = 1;
    info.flags.z = 0;
    info.flags.rcode = SESE_DNS_RCODE_NAME_ERROR;

    info.questions = 1;
    info.answerPrs = 0;
    info.authorityPrs = 1;
    info.additionalPrs = 0;

    sese::net::dns::DNSUtil::encodeFrameHeaderInfo(result, info);
    EXPECT_EQ(memcmp(expect, result, 12), 0);
}

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::localhost(port);
}

TEST(TestDNS, DnsClient) {
    auto server = sese::net::IPv4Address::create("114.114.114.114", 53);
    auto ip = sese::net::dns::DnsClient::resolveAuto("www.bing.com", server);
    ASSERT_NE(ip, nullptr);
    SESE_INFO("www.bing.com -> %s", ip->getAddress().c_str());
}

TEST(TestDNS, Server) {
    auto addr = createAddress();

    sese::net::dns::DNSConfig config;
    config.address = addr;
    config.hostIPv4Map["www.example.com"] = "127.0.0.1";
    config.hostIPv4Map["www.kaoru.com"] = "192.168.3.230";
    config.hostIPv6Map["www.kaoru.com"] = "::1";

    auto server = sese::net::dns::DnsServer::create(&config);
    ASSERT_NE(server, nullptr);

    server->start();

    auto ipv4 = sese::net::dns::DnsClient::resolveAuto("www.kaoru.com", addr);
    ASSERT_NE(ipv4, nullptr);
    SESE_INFO("www.kaoru.com -> %s", ipv4->getAddress().c_str());

    auto ipv6 = sese::net::dns::DnsClient::resolveAuto("www.kaoru.com", addr, AF_INET6);
    ASSERT_NE(ipv6, nullptr);
    SESE_INFO("www.kaoru.com -> %s", ipv6->getAddress().c_str());

    server->shutdown();
}