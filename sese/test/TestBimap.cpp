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

#include "sese/container/Bimap.h"
#include "sese/log/Marco.h"
#include "gtest/gtest.h"

TEST(TestBimap, ConstMethod) {
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
        SESE_DEBUG("Key: {} Value: {}", key.c_str(), value);
    }
}