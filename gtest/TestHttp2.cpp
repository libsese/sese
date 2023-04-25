#include "sese/net/http/V2Http2ServerOption.h"
#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/Huffman.h"
#include "gtest/gtest.h"

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
    const char buf[] = "\xdc\x34\xfd\x28\x00\xa9\x0d\x76\x28\x20\x09\x95\x02\xd5\xc6\xdd\xb8\xcb\x2a\x62\xd1\xbf";

    sese::net::http::HuffmanEncoder encoder;
    auto code = encoder.encode("Sat, 01 Apr 2023 14:57:33 GMT");
    ASSERT_TRUE(!code.empty());

    std::string_view bufView {buf, sizeof(buf) - 1};
    std::string_view codeView { (const char *) code.data(), code.size()};
    EXPECT_TRUE(bufView == codeView);
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

TEST(TestHttp2, Http2ServerOption_0) {
    sese::net::v2::http::Http2ServerOption option(4);
}