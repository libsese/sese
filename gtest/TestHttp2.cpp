#include "sese/net/http/DynamicTable.h"
#include "gtest/gtest.h"

TEST(TestHttp2, DynamicTable_0) {
    sese::http::DynamicTable table(15);
    table.set("k1", "v1");
    table.set("k2", "v2");
    EXPECT_TRUE(table.getCount() == 1);
    EXPECT_TRUE(table.getSize() == 8);
    auto item = table.get(62);
    ASSERT_TRUE(item != std::nullopt);
    EXPECT_TRUE(item->second == "v2");
}