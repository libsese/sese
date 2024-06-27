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
        std::string_view code_view{(const char *) code.data(), code.size()};
        EXPECT_TRUE(buf_view == code_view);
    }
    {
        const char BUF[] = "\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b\x5f\xcf";
        auto code = encoder.encode("\"63ea2d3e-18b4\"");
        ASSERT_TRUE(!code.empty());

        std::string_view buf_view{BUF, sizeof(BUF) - 1};
        std::string_view code_view{(const char *) code.data(), code.size()};
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
    const char BUF[] = "\x88\x61\x96\xdc\x34\xfd\x28"
                       "\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62"
                       "\xd1\xbf\x5f\x87\x49\x7c\xa5\x89\xd3\x4d\x1f\x6c\x96\xd0\x7a\xbe"
                       "\x94\x0b\x2a\x61\x2c\x6a\x08\x02\x65\x40\x8a\xe0\x9f\xb8\xd8\x14"
                       "\xc5\xa3\x7f\x0f\x13\x8c\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b"
                       "\x5f\xcf\x52\x84\x8f\xd2\x4a\x8f\x0f\x0d\x83\x71\x91\x35\x40\x8f"
                       "\xf2\xb4\x63\x27\x52\xd5\x22\xd3\x94\x72\x16\xc5\xac\x4a\x7f\x86"
                       "\x02\xe0\x00\x80\xc8\xbf\x76\x86\xaa\x69\xd2\x9a\xfc\xff\x40\x85"
                       "\x1d\x09\x59\x1d\xc9\xa1\x9d\x98\x3f\x9b\x8d\x34\xcf\xf3\xf6\xa5"
                       "\x23\x81\x97\x00\x0f\xa5\x27\x65\x61\x3f\x07\xf3\x71\xa6\x99\xfe"
                       "\x7e\xd4\xa4\x70\x32\xe0\x01\x7c\x87\x12\x95\x4d\x3a\x53\x5f\x9f"
                       "\x40\x8b\xf2\xb4\xb6\x0e\x92\xac\x7a\xd2\x63\xd4\x8f\x89\xdd\x0e"
                       "\x8c\x1a\xb6\xe4\xc5\x93\x4f\x40\x8c\xf2\xb7\x94\x21\x6a\xec\x3a"
                       "\x4a\x44\x98\xf5\x7f\x8a\x0f\xda\x94\x9e\x42\xc1\x1d\x07\x27\x5f"
                       "\x40\x90\xf2\xb1\x0f\x52\x4b\x52\x56\x4f\xaa\xca\xb1\xeb\x49\x8f"
                       "\x52\x3f\x85\xa8\xe8\xa8\xd2\xcb";

    auto input = sese::io::InputBufferWrapper(BUF, sizeof(BUF) - 1);
    auto table = sese::net::http::DynamicTable();
    auto header = sese::net::http::Header();

    ASSERT_TRUE(sese::net::http::HPackUtil::decode(&input, sizeof(BUF) - 1, table, header));

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
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header));

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
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header));

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
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, resp_table, resp_header));

        showStreamHeader(resp_header);
        showDynamicTable(resp_table);
    }
}