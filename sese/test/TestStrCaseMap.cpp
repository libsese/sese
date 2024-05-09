#include <sese/container/StrCaseMap.h>
#include <gtest/gtest.h>

TEST(TestStrCaseMap, Map) {
    sese::StrCaseMap<int> map;
    map["ABC"] = 1;
    {
        auto iterator = map.find("abc");
        ASSERT_NE(iterator, map.end());
        EXPECT_EQ(iterator->second, 1);
    }
    {
        auto iterator = map.find("CBA");
        ASSERT_EQ(iterator, map.end());
    }
}

TEST(TestStrCaseMap, UnorderedMap) {
    sese::StrCaseUnorderedMap<int> map;
    map["ABC"] = 1;
    {
        auto iterator = map.find("abc");
        ASSERT_NE(iterator, map.end());
        EXPECT_EQ(iterator->second, 1);
    }
    {
        auto iterator = map.find("CBA");
        ASSERT_EQ(iterator, map.end());
    }
}