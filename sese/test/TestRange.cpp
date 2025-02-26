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

#include <sese/log/Marco.h>
#include <sese/util/Range.h>

#include <gtest/gtest.h>

#include <algorithm>

TEST(TestRange, ForEach_1) {
    for (auto &&item: sese::Range<int64_t>(5)) {
        SESE_INFO("item: {}", item);
    }
}

TEST(TestRange, ForEach_2) {
    sese::Range<int32_t> range(5);
    auto iterator = range.begin();
    for (; iterator != range.end(); ++iterator) {
        SESE_INFO("iter: {}", *iterator);
    }
}

TEST(TestRange, ReverseForEach_1) {
    sese::Range<int64_t> range(-3, 4);
    auto iterator = range.rbegin();
    for (; iterator != range.rend(); ++iterator) {
        SESE_INFO("iter: {}", *iterator);
    }
}

TEST(TestRange, ReverseForEach_2) {
    sese::Range<int64_t> range(-12, -7);
    std::for_each(range.rbegin(), range.rend(), [](const int64_t &number) {
        SESE_INFO("iter: {}", number);
    });
}

TEST(TestRange, Construct_1) {
    sese::Range<int64_t> range(10);
    for (auto &&item: range) {
        SESE_INFO("item: {}", item);
    }
}

TEST(TestRange, Construct_2) {
    sese::Range<int64_t> range(20, 27);
    for (auto &&item: range) {
        SESE_INFO("item: {}", item);
    }
}

TEST(TestRange, Construct_3) {
    sese::Range<size_t> range(19, 16);
    for (auto &&item: range) {
        SESE_INFO("item: {}", item);
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