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

#include "sese/util/ArgParser.h"
#include "sese/log/Marco.h"
#include "sese/system/CommandLine.h"
#include "gtest/gtest.h"

TEST(TestArgParser, Parse_0) {
    int argc = 4;
    auto arg0 = sese::system::CommandLine::getArgv()[0];
    auto arg1 = "a1=hello";
    auto arg2 = "a2=\"a sentence\"";
    auto arg3 = "a3=123";
    char *argv[] = {
        const_cast<char *>(arg0),
        const_cast<char *>(arg1),
        const_cast<char *>(arg2),
        const_cast<char *>(arg3)
    };

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));

    SESE_INFO("full path: {}", args->getFullPath().c_str());
    SESE_INFO("current path: {}", args->getCurrentPath().c_str());
    SESE_INFO("exec file name: {}", args->getFileName().c_str());

    auto find_value = [&args](const std::string &key, const std::string &default_value) {
        SESE_INFO("{} = {}", key.c_str(), args->getValueByKey(key, default_value).c_str());
    };

    find_value("a1", "undef");
    find_value("a2", "undef");
    find_value("a3", "undef");
    find_value("a4", "undef");

    auto set = args->getKeyValSet();
    for (auto &item: set) {
        SESE_INFO("{}: {}", item.first.c_str(), item.second.c_str());
    }
}

TEST(TestArgParser, Parse_1) {
    int argc = 2;
    auto arg0 = sese::system::CommandLine::getArgv()[0];
    auto arg1 = "--enable-xxx";
    char *argv[] = {
        const_cast<char *>(arg0),
        const_cast<char *>(arg1)
    };

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));
    EXPECT_TRUE(args->exist("--enable-xxx"));
}

TEST(TestArgParser, Parse_3) {
    int argc = 1;
    auto arg0 = sese::system::CommandLine::getArgv()[0];
    char *argv[] = {const_cast<char *>(arg0)};

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));
}
