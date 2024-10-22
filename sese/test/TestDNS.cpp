#include <sese/record/Marco.h>
#include <sese/net/dns/DnsPackage.h>

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