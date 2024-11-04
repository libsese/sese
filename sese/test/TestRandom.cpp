// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define SESE_C_LIKE_FORMAT

#include "sese/config/CSVWriter.h"
#include "sese/io/FileStream.h"
#include "sese/util/RandomUtil.h"
#include "sese/record/Marco.h"

#include "gtest/gtest.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "sese/util/Random.h"

TEST(TestRandom, STD) {
    sese::Random device;
    ASSERT_EQ(device.entropy(), 64);
    std::uniform_int_distribution<int> distribution(-150000, 150000);

    auto file = sese::io::FileStream::create("random.csv", "wt");
    ASSERT_NE(file, nullptr);
    auto writer = sese::CSVWriter(file.get(), ',', false);
    for (int i = 0; i < 300; i++) {
        std::default_random_engine engine(static_cast<unsigned int>(device()));
        writer.write({std::to_string(i), std::to_string(distribution(engine))});
    }
    file->close();
}

TEST(TestRandom, Misc) {
    ASSERT_EQ(sese::Random::max(), UINT64_MAX);
    ASSERT_EQ(sese::Random::min(), 0);
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

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

TEST(TestRandomUtil, String_1) {
    using sese::RandomUtil;
    size_t len = 11;
    auto str = sese::RandomUtil::nextString(len, RandomUtil::REQUIRED_DIGIT | RandomUtil::REQUIRED_LOWER_LETTER);
    SESE_INFO("random string: %s", str.c_str());
    EXPECT_EQ(str.length(), len);
}

TEST(TestRandomUtil, String_2) {
    using sese::RandomUtil;
    size_t len = 17;
    auto str = sese::RandomUtil::nextString(len, RandomUtil::REQUIRED_DIGIT | RandomUtil::REQUIRED_UPPER_LETTER | RandomUtil::REQUIRED_SYMBOL);
    SESE_INFO("random string: %s", str.c_str());
    EXPECT_EQ(str.length(), len);
}

TEST(TestRandomUtil, String_3) {
    using sese::RandomUtil;
    size_t len = 23;
    auto str = sese::RandomUtil::nextString(len, RandomUtil::REQUIRED_DIGIT | RandomUtil::REQUIRED_UPPER_LETTER | RandomUtil::REQUIRED_LOWER_LETTER | RandomUtil::REQUIRED_SYMBOL);
    SESE_INFO("random string: %s", str.c_str());
    EXPECT_EQ(str.length(), len);
}

TEST(TestRandomUtil, String_Empty) {
    using sese::RandomUtil;
    size_t len = 23;
    auto str = sese::RandomUtil::nextString(len, 0);
    EXPECT_TRUE(str.empty());
}