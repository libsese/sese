#include "sese/container/Bimap.h"
#include "sese/record/Marco.h"
#include "gtest/gtest.h"

TEST(TestBitmap, ConstMethod) {
    sese::Bimap<std::string, int32_t> bimap{{"1", 1}};

    EXPECT_FALSE(bimap.empty());
    EXPECT_EQ(bimap.size(), 1);

    EXPECT_TRUE(bimap.existByKey("1"));
    EXPECT_TRUE(bimap.existByValue(1));
    EXPECT_FALSE(bimap.existByKey("2"));

    EXPECT_EQ(bimap.getByKey("1"), 1);
    EXPECT_EQ(bimap.getByValue(1), "1");
}

TEST(TestBimap, Method) {
    sese::Bimap<std::string, int32_t> bimap;
    bimap.insert("1", 1);
    bimap.insert("2", 2);
    bimap.insert("3", 3);

    EXPECT_TRUE(bimap.tryEraseByKey("1"));
    EXPECT_FALSE(bimap.tryEraseByKey("1"));

    EXPECT_TRUE(bimap.tryEraseByValue(2));
    EXPECT_FALSE(bimap.tryEraseByValue(2));

    bimap.clear();
}

TEST(TestBimap, Iterator) {
    sese::Bimap<std::string, int32_t> bimap{
            {"Hello", 1},
            {"World", 2}
    };

    for (const auto& [key, value]: bimap) {
        SESE_DEBUG("Key: %s Value: %d", key.c_str(), value);
    }
}