#include <sese/record/Marco.h>
#include <sese/util/Range.h>
#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

TEST(TestRange, ForEach_1) {
    for (auto &&item: sese::Range(5)) {
        SESE_INFO("item: %ld", item);
    }
}

TEST(TestRange, ForEach_2) {
    sese::Range range(5);
    auto iterator = range.begin();
    for (; iterator != range.end(); ++iterator) {
        SESE_INFO("iter: %ld", *iterator);
    }
}

TEST(TestRange, ReverseForEach_1) {
    sese::Range range(-3, 4);
    auto iterator = range.rbegin();
    for (; iterator != range.rend(); ++iterator) {
        SESE_INFO("iter: %ld", *iterator);
    }
}

TEST(TestRange, ReverseForEach_2) {
    sese::Range range(-12, -7);
    std::for_each(range.rbegin(), range.rend(), [](decltype(range)::ReverseIterator::reference number){
        SESE_INFO("iter: %ld", number);
    });
}

TEST(TestRange, Construct_1) {
    sese::Range range(10);
    for (auto &&item: range) {
        SESE_INFO("item: %ld", item);
    }
}

TEST(TestRange, Construct_2) {
    sese::Range range(20, 27);
    for (auto &&item: range) {
        SESE_INFO("item: %ld", item);
    }
}

TEST(TestRange, Construct_3) {
    sese::Range range(19, 16);
    for (auto &&item: range) {
        SESE_INFO("item: %ld", item);
    }
}

TEST(TestRange, Exist_1) {
    sese::Range range1(5, 9);
    sese::Range range2(-3, 4);
    sese::Range range3(-7, 0);

    EXPECT_TRUE(range1.exist(5));
    EXPECT_TRUE(range1.exist(7));
    EXPECT_FALSE(range1.exist(10));
    EXPECT_FALSE(range1.exist(4));

    EXPECT_TRUE(range2.exist(-3));
    EXPECT_TRUE(range2.exist(0));
    EXPECT_TRUE(range2.exist(4));
    EXPECT_FALSE(range2.exist(-4));
    EXPECT_FALSE(range2.exist(5));

    EXPECT_TRUE(range3.exist(0));
    EXPECT_TRUE(range3.exist(-7));
    EXPECT_FALSE(range3.exist(1));
    EXPECT_FALSE(range3.exist(-8));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}