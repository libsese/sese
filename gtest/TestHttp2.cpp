#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif _WIN32
#pragma warning(disable : 4996)
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/net/http/V2Http2Server.h"
#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/Huffman.h"
#include "sese/system/Process.h"
#include "sese/util/InputBufferWrapper.h"
#include "sese/util/Random.h"
#include "gtest/gtest.h"

auto makeRandomPortAddr() {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024) + 1024);
    printf("select port %d\n", port);
    auto addr = sese::net::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

void showStreamHeader(auto &header) noexcept {
    puts("============ Stream Header ============");
    for (decltype(auto) pair: header) {
        printf("%s: %s\n", pair.first.c_str(), pair.second.c_str());
    }
}

void showStreamHeader(auto &onceHeader, auto &indexedHeader) noexcept {
    puts("============ Stream Header ============");
    for (decltype(auto) pair: onceHeader) {
        printf("%s: %s\n", pair.first.c_str(), pair.second.c_str());
    }
    for (decltype(auto) pair: indexedHeader) {
        printf("%s: %s\n", pair.first.c_str(), pair.second.c_str());
    }
}

void showDynamicTable(const auto &table) noexcept {
    puts("============ Dynamic Table ============");
    for (decltype(auto) pair: table) {
        printf("%s: %s\n", pair.first.c_str(), pair.second.c_str());
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
        const char buf[] = "\xdc\x34\xfd\x28\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62\xd1\xbf";
        auto code = encoder.encode("Sat, 01 Apr 2023 14:57:33 GMT");
        ASSERT_TRUE(!code.empty());

        std::string_view bufView{buf, sizeof(buf) - 1};
        std::string_view codeView{(const char *) code.data(), code.size()};
        EXPECT_TRUE(bufView == codeView);
    }
    {
        const char buf[] = "\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b\x5f\xcf";
        auto code = encoder.encode("\"63ea2d3e-18b4\"");
        ASSERT_TRUE(!code.empty());

        std::string_view bufView{buf, sizeof(buf) - 1};
        std::string_view codeView{(const char *) code.data(), code.size()};
        EXPECT_TRUE(bufView == codeView);
    }
}

TEST(TestHttp2, HuffmanDecoder) {
    sese::net::http::HuffmanDecoder decoder;

    const char buf1[] = "\xdc\x34\xfd\x28\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62\xd1\xbf";
    auto str1 = decoder.decode(buf1, sizeof(buf1) - 1);
    ASSERT_TRUE(str1 != std::nullopt);
    EXPECT_TRUE(str1.value() == "Sat, 01 Apr 2023 14:57:33 GMT");

    const char buf2[] = "\xfe\x5c\x64\xa3\x14\x8c\x95\x60\xbd\x1b\x5f\xcf";
    auto str2 = decoder.decode(buf2, sizeof(buf2) - 1);
    ASSERT_TRUE(str2 != std::nullopt);
    EXPECT_TRUE(str2.value() == "\"63ea2d3e-18b4\"");
}

TEST(TestHttp2, DISABLE_HPackDecode) {
    const char buf[] = "\x88\x61\x96\xdc\x34\xfd\x28"
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

    auto input = sese::InputBufferWrapper(buf, sizeof(buf) - 1);
    auto table = sese::net::http::DynamicTable();
    auto header = sese::net::http::Header();

    ASSERT_TRUE(sese::net::http::HPackUtil::decode(&input, sizeof(buf) - 1, table, header));

    showStreamHeader(header);
    showDynamicTable(table);
}

TEST(TestHttp2, HeaderExample) {
    auto buffer = sese::ByteBuilder();
    auto reqTable = sese::net::http::DynamicTable();
    auto respTable = sese::net::http::DynamicTable();
    size_t size;

    {
        puts("1st request");
        auto onceHeader = sese::net::http::Header();
        auto indexedHeader = sese::net::http::Header();
        onceHeader.set(":method", "GET");
        onceHeader.set(":scheme", "http");
        onceHeader.set(":path", "/");
        indexedHeader.set(":authority", "www.example.com");
        size = sese::net::http::HPackUtil::encode(&buffer, reqTable, onceHeader, indexedHeader);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(onceHeader, indexedHeader);
        showDynamicTable(reqTable);

        auto respHeader = sese::net::http::Header();
        buffer.write(raw, size);
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, respTable, respHeader));

        showStreamHeader(respHeader);
        showDynamicTable(respTable);
    }

    {
        puts("\n2nd request");
        auto onceHeader = sese::net::http::Header();
        auto indexedHeader = sese::net::http::Header();
        onceHeader.set(":method", "GET");
        onceHeader.set(":scheme", "http");
        onceHeader.set(":path", "/");
        indexedHeader.set(":authority", "www.example.com");
        indexedHeader.set("cache-control", "no-cache");
        size = sese::net::http::HPackUtil::encode(&buffer, reqTable, onceHeader, indexedHeader);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(onceHeader, indexedHeader);
        showDynamicTable(reqTable);

        auto respHeader = sese::net::http::Header();
        buffer.write(raw, size);
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, respTable, respHeader));

        showStreamHeader(respHeader);
        showDynamicTable(respTable);
    }

    {
        puts("\n3rd request");
        auto onceHeader = sese::net::http::Header();
        auto indexedHeader = sese::net::http::Header();
        onceHeader.set(":method", "GET");
        onceHeader.set(":scheme", "http");
        onceHeader.set(":path", "/index.html");
        indexedHeader.set(":authority", "www.example.com");
        indexedHeader.set("custom-key", "custom-value");
        size = sese::net::http::HPackUtil::encode(&buffer, reqTable, onceHeader, indexedHeader);

        char raw[1024];
        buffer.read(raw, size);
        showStreamHeader(onceHeader, indexedHeader);
        showDynamicTable(reqTable);

        auto respHeader = sese::net::http::Header();
        buffer.write(raw, size);
        ASSERT_TRUE(sese::net::http::HPackUtil::decode(&buffer, size, respTable, respHeader));

        showStreamHeader(respHeader);
        showDynamicTable(respTable);
    }
}

TEST(TestHttp2, Server) {
    auto addr = makeRandomPortAddr();
    ASSERT_TRUE(addr != nullptr);

    sese::net::v2::http::Http2Server server;
    server.setBindAddress(addr);
    ASSERT_TRUE(server.init());
    server.start();

    auto cmd = PY_EXECUTABLE " " PROJECT_PATH "/scripts/do_http2_request.py " + std::to_string(addr->getPort());
    auto process = sese::system::Process::create(cmd.c_str());
    EXPECT_EQ(process->wait(), 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    server.shutdown();
}

#ifdef __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#endif