#include <sese/record/Marco.h>
#include <sese/net/dns/DnsPackage.h>
#include <sese/service/dns/DnsServer.h>
#include <sese/net/dns/Resolver.h>
#include <sese/system/ProcessBuilder.h>
#include <sese/net/Socket.h>

#include <array>
#include <random>

#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(TestDNS, DecodeWords) {
    std::array<uint8_t, 55> array = {
        0x12, 0x34, // ID = 0x1234
        0x01, 0x00, // Flags = 0x0100 (Standard query)
        0x00, 0x03, // QDCOUNT = 3 (3 questions)
        0x00, 0x00, // ANCOUNT = 0
        0x00, 0x00, // NSCOUNT = 0
        0x00, 0x00, // ARCOUNT = 0

        // Question 1: www.example.com
        // Count: 21 bytes
        0x03, 'w', 'w', 'w',
        0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e',
        0x03, 'c', 'o', 'm',
        0x00,
        0x00, 0x00, 0x00, 0x00,

        // Question 2: mail.example.com
        // Count: 12 bytes
        0x04, 'm', 'a', 'i', 'l',
        0xc0, 0x10,
        0x00,
        0x00, 0x00, 0x00, 0x00,

        // Question 3: my.mail.example.com
        // Count: 10 bytes
        0x02, 'm', 'y',
        0xc0, 0x21,
        0x00,
        0x00, 0x00, 0x00, 0x00
    };

    std::array<std::string, 3> expect = {"www.example.com", "mail.example.com", "my.mail.example.com"};
    auto pkg = sese::net::dns::DnsPackage::decode(array.data(), array.size());

    auto &&items = pkg->getQuestions();
    for (int i = 0; i < items.size(); i++) {
        EXPECT_EQ(items[i].name, expect[i]);
    }
}

TEST(TestDNS, Encode) {
    auto pkg = sese::net::dns::DnsPackage::new_();
    auto &&questions = pkg->getQuestions();
    questions.push_back({"www.example.com", 1, 1});
    questions.push_back({"mail.example.com", 1, 1});

    auto index = pkg->buildIndex();

    size_t compressed_length = 0;
    pkg->encode(nullptr, compressed_length, index);

    auto no_index = sese::net::dns::DnsPackage::Index();
    size_t length = 0;
    pkg->encode(nullptr, length, no_index);

    SESE_INFO("compressed length: {}", compressed_length);
    SESE_INFO("length: {}", length);
}

TEST(TestDNS, Resolver) {
    auto port = sese::net::createRandomPort();
    sese::net::dns::Resolver resolver;
    resolver.addNameServer("127.0.0.1", port);
    resolver.addNameServer("8.8.8.8", 53);

    auto process = sese::system::ProcessBuilder(PY_EXECUTABLE)
                       .args(PROJECT_PATH "/scripts/dns_server.py")
                       .args(std::to_string(port))
                       .create();
    ASSERT_NE(nullptr, process);
    sese::sleep(1s);

    auto result = resolver.resolve("www.example.com", 1);
    for (auto &&i: result) {
        if (i->getFamily() == AF_INET) {
            auto ipv4 = std::dynamic_pointer_cast<sese::net::IPv4Address>(i);
            SESE_INFO("{}", ipv4->getAddress());
        }
    }

    result = resolver.resolve("bing.com", 1);
    for (auto &&i: result) {
        if (i->getFamily() == AF_INET) {
            auto ipv4 = std::dynamic_pointer_cast<sese::net::IPv4Address>(i);
            SESE_INFO("{}", ipv4->getAddress());
        }
    }

    EXPECT_TRUE(process->kill());
}

TEST(TestDNS, Server) {
    auto port = sese::net::createRandomPort();
    sese::service::dns::DnsServer server;
    server.addUpstreamNameServer("8.8.8.8");
    server.addRecord("www.example.com", sese::net::IPv4Address::localhost());
    ASSERT_TRUE(server.bind(sese::net::IPv4Address::localhost(port)));
    server.startup();

    auto process = sese::system::ProcessBuilder(PY_EXECUTABLE)
                      .args(PROJECT_PATH "/scripts/dns_client.py")
                      .args(std::to_string(port))
                      .create();
    ASSERT_NE(nullptr, process);
    sese::sleep(1s);
    EXPECT_EQ(0, process->wait());

    server.shutdown();
}