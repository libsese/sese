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

#include <sese/util/Result.h>
#include <gtest/gtest.h>

TEST(TestResult, ResultIsVoid) {
    auto func1 = [] {
        return sese::Result<void, int>::error(10);
    };

    auto func2 = [] {
        return sese::Result<void, int>::success();
    };

    auto rt = func1();
    EXPECT_TRUE(rt);
    EXPECT_EQ(rt.err(), 10);
    EXPECT_FALSE(func2());
}

TEST(TestResult, ErrorIsVoid) {
    auto func1 = [] {
        return sese::Result<int, void>::error();
    };
    auto func2 = [] {
        return sese::Result<int, void>::success(114);
    };

    EXPECT_TRUE(func1());
    auto rt = func2();
    EXPECT_FALSE(rt);
    EXPECT_EQ(rt.get(), 114);
}

TEST(TestResult, BothAreVoid) {
    auto func1 = [] {
        return sese::Result<void, void>::error();
    };
    auto func2 = [] {
        return sese::Result<void, void>::success();
    };
    EXPECT_TRUE(func1());
    EXPECT_FALSE(func2());
}