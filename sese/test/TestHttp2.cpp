#define SESE_C_LIKE_FORMAT

#include "sese/io/ByteBuilder.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/HPackUtil.h"
#include "sese/net/http/Huffman.h"
#include "sese/record/Marco.h"
#include "sese/service/BalanceLoader.h"
#include "sese/system/Process.h"

#include "gtest/gtest.h"

#define printf SESE_INFO

static auto makeRandomPortAddr() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", port);
    auto addr = sese::net::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

void showStreamHeader(auto &header) noexcept {
    puts("============ Stream Header ============");
    for (decltype(auto) pair: header) {
        printf("%s: %s", pair.first.c_str(), pair.second.c_str());
    }
}

void showStreamHeader(auto &once_header, auto &indexed_header) noexcept {
    puts("============ Stream Header ============");
    for (decltype(auto) pair: once_header) {
        printf("%s: %s", pair.first.c_str(), pair.second.c_str());
    }
    for (decltype(auto) pair: indexed_header) {
        printf("%s: %s", pair.first.c_str(), pair.second.c_str());
    }
}

void showDynamicTable(const auto &table) noexcept {
    puts("============ Dynamic Table ============");
    for (decltype(auto) pair: table) {
        printf("%s: %s", pair.first.c_str(), pair.second.c_str());
    }
}

TEST(TestHttp2, DynamicTable_0) {
    sese::net::http::DynamicTable table(15);
    table.set("k1", "v1");
    table.set("k2", "v2");
    EXPECT_TRUE(table.getCount() == 1);
    EXPECT_TRUE(table.getSize() == 8);
    auto item = table.get(62);
    ASSERT_TRUE(item != std::nullopt);
    EXPECT_TRUE(item->second == "v2");
}

TEST(TestHttp2, HuffmanEncoder) {
    sese::net::http::HuffmanEncoder encoder;
    {
        const char BUF[] = "\xdc\x34\xfd\x28\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62\xd1\xbf";
        auto code = encoder.encode("Sat, 01 Apr 2023 14:57:33 GMT");
        ASSERT_TRUE(!code.empty());

        std::string_view buf_view{BUF, sizeof(BUF) - 1};
        std::string_view code_view{reinterpret_cast<const char *>(code.data()), code.size()};
        EXPECT_TRUE(buf_view == code_view);
    }
    {
        const char BUF[] = "\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b\x5f\xcf";
        auto code = encoder.encode("\"63ea2d3e-18b4\"");
        ASSERT_TRUE(!code.empty());

        std::string_view buf_view{BUF, sizeof(BUF) - 1};
        std::string_view code_view{reinterpret_cast<const char *>(code.data()), code.size()};
        EXPECT_TRUE(buf_view == code_view);
    }
}

TEST(TestHttp2, HuffmanDecoder) {
    sese::net::http::HuffmanDecoder decoder;

    const char BUF1[] = "\xdc\x34\xfd\x28\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62\xd1\xbf";
    auto str1 = decoder.decode(BUF1, sizeof(BUF1) - 1);
    ASSERT_TRUE(str1 != std::nullopt);
    EXPECT_TRUE(str1.value() == "Sat, 01 Apr 2023 14:57:33 GMT");

    const char BUF2[] = "\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b\x5f\xcf";
    auto str2 = decoder.decode(BUF2, sizeof(BUF2) - 1);
    ASSERT_TRUE(str2 != std::nullopt);
    EXPECT_TRUE(str2.value() == "\"63ea2d3e-18b4\"");
}

TEST(TestHttp2, HPackDecode) {
    const uint8_t BUF[] = {0x82, 0x86, 0x41, 0x88, 0xaa, 0x69, 0xd2, 0x9a, 0xc4, 0xb9, 0xec, 0x9b, 0x84, 0x7a, 0x88, 0x25, 0xb6, 0x50, 0xc3, 0xcb, 0xbe, 0xb8, 0x7f, 0x53, 0x3, 0x2a, 0x2f, 0x2a};

    auto input = sese::io::InputBufferWrapper(reinterpret_cast<const char *>(BUF), sizeof(BUF));
    auto table = sese::net::http::DynamicTable();
    auto header = sese::net::http::Header();

    ASSERT_EQ(0, sese::net::http::HPackUtil::decode(&input, sizeof(BUF), table, header, false));

    showStreamHeader(header);
    showDynamicTable(table);
}

TEST(TestHttp2, HeaderExample) {
    auto buffer = sese::io::ByteBuilder();
    auto req_table = sese::net::http::DynamicTable();
    auto resp_table = sese::net::http::DynamicTable();
    size_t size;

    {
        puts("1st request");
        auto once_header = sese::net::http::Header();
        auto indexed_header = sese::net::http::Header();
        once_header.set(":method", "GET");
        once_header.set(":scheme", "http");
        once_header.set(":path", "/");
        indexed_header.set(":authority", "www.example.com");
        size = sese::net::http::HPackUtil::encode(&buffer, req_table, once_header, indexed_header);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(once_header, indexed_header);
        showDynamicTable(req_table);

        auto resp_header = sese::net::http::Header();
        buffer.write(raw, size);
        ASSERT_EQ(0, sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header, false));

        showStreamHeader(resp_header);
        showDynamicTable(resp_table);
    }

    {
        puts("\n2nd request");
        auto once_header = sese::net::http::Header();
        auto indexed_header = sese::net::http::Header();
        once_header.set(":method", "GET");
        once_header.set(":scheme", "http");
        once_header.set(":path", "/");
        indexed_header.set(":authority", "www.example.com");
        indexed_header.set("cache-control", "no-cache");
        size = sese::net::http::HPackUtil::encode(&buffer, req_table, once_header, indexed_header);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(once_header, indexed_header);
        showDynamicTable(req_table);

        auto resp_header = sese::net::http::Header();
        buffer.write(raw, size);
        ASSERT_EQ(0, sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header, false));

        showStreamHeader(resp_header);
        showDynamicTable(resp_table);
    }

    {
        puts("\n3rd request");
        auto once_header = sese::net::http::Header();
        auto indexed_header = sese::net::http::Header();
        once_header.set(":method", "GET");
        once_header.set(":scheme", "http");
        once_header.set(":path", "/index.html");
        indexed_header.set(":authority", "www.example.com");
        indexed_header.set("custom-key", "custom-value");
        size = sese::net::http::HPackUtil::encode(&buffer, req_table, once_header, indexed_header);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(once_header, indexed_header);
        showDynamicTable(req_table);

        auto resp_header = sese::net::http::Header();
        buffer.write(raw, size);
        EXPECT_EQ(0, sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header, false));

        showStreamHeader(resp_header);
        showDynamicTable(resp_table);
    }
}