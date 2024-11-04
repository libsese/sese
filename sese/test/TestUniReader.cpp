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

#include <gtest/gtest.h>

#include <sese/config/UniReader.h>
#include <sese/record/Marco.h>

TEST(TestUniReader, Open_0) {
    auto reader = sese::UniReader::create(PROJECT_PATH "/sese/test/Data/data-0.txt");
    GTEST_ASSERT_NE(reader, nullptr);

    std::string str;
    while (!(str = reader->readLine()).empty()) {
        SESE_INFO("%s", str.c_str());
    }
}

TEST(TestUniReader, Open_1) {
    GTEST_ASSERT_EQ(sese::UniReader::create("undef.txt"), nullptr);
}