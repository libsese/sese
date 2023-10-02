#include "sese/config/CSVWriter.h"
#include "sese/io/FileStream.h"
#include "sese/util/RandomUtil.h"
#include "sese/record/Marco.h"

#include "gtest/gtest.h"

#include <random>

TEST(TestRandom, STD) {
    sese::Random device;
    ASSERT_EQ(device.entropy(), 64);
    std::uniform_int_distribution<int> distribution(-150000, 150000);

    auto file = sese::io::FileStream::create("random.csv", "wt");
    ASSERT_NE(file, nullptr);
    auto writer = sese::CSVWriter(file.get(), ',', false);
    for (int i = 0; i < 300; i++) {
        std::default_random_engine engine((unsigned int) device());
        writer.write({std::to_string(i), std::to_string(distribution(engine))});
    }
    file->close();
}

TEST(TestRandom, Misc) {
    ASSERT_EQ(sese::Random::max(), UINT64_MAX);
    ASSERT_EQ(sese::Random::min(), 0);
}

TEST(TestRandomUtil, Signed) {
    auto i1 = sese::RandomUtil::next<int>();
    SESE_INFO("random signed integer: %d", i1);

    auto i2 = sese::RandomUtil::next<int>(20, 30);
    SESE_INFO("random signed integer: %d", i2);
    EXPECT_TRUE(i2 >= 20 && i2 <= 30);
}

TEST(TestRandomUtil, Unsigned) {
    auto i1 = sese::RandomUtil::next<unsigned>();
    SESE_INFO("random unsigned integer: %u", i1);

    auto i2 = sese::RandomUtil::next<unsigned>(20, 30);
    SESE_INFO("random unsigned integer: %u", i2);
    EXPECT_TRUE(i2 >= 20 && i2 <= 30);
}

TEST(TestRandomUtil, Float) {
    auto f1 = sese::RandomUtil::next<float>();
    SESE_INFO("random float: %f", f1);

    auto f2 = sese::RandomUtil::next<float>(114.514f, 191.810f);
    SESE_INFO("random float: %f", f2);
    EXPECT_TRUE(f2 >= 114.514f && f2 <= 191.810f);
}

TEST(TestRandomUtil, Double) {
    auto d1 = sese::RandomUtil::next<double>();
    SESE_INFO("random double: %f", d1);

    auto d2 = sese::RandomUtil::next<double>(114.514f, 191.810f);
    SESE_INFO("random double: %f", d2);
    EXPECT_TRUE(d2 >= 114.514f && d2 <= 191.810f);
}