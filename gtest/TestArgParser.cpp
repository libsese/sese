#include "sese/util/ArgParser.h"
#include "sese/record/Marco.h"
#include "gtest/gtest.h"

TEST(TestArgParser, Parse_0) {
    int argc = 4;
    char arg0[32]{R"(D:\Users\sese\TestArgParser)"};
    // char arg0[32] {"/home/sese/TestArgParser"};
    char arg1[16]{"a1=hello"};
    char arg2[16]{"a2=\"a sentence\""};
    char arg3[16]{"a3=123"};
    char *argv[] = {arg0, arg1, arg2, arg3};

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));

    SESE_INFO("full path: %s", args->getFullPath().c_str());
    SESE_INFO("current path: %s", args->getCurrentPath().c_str());
    SESE_INFO("exec file name: %s", args->getFileName().c_str());

    auto findValue = [&args](const std::string &key, const std::string &defaultValue) {
        SESE_INFO("%s = %s", key.c_str(), args->getValueByKey(key, defaultValue).c_str());
    };

    findValue("a1", "undef");
    findValue("a2", "undef");
    findValue("a3", "undef");
    findValue("a4", "undef");

    auto set = args->getKeyValSet();
    for (auto &item : set) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
    }
}

TEST(TestArgParser, Parse_1) {
    int argc = 2;
    char arg0[32]{R"(D:\Users\sese\TestArgParser)"};
    char arg1[16]{"--enable-xxx"};
    char *argv[] = {arg0, arg1};

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_FALSE(args->parse(argc, argv));
}

TEST(TestArgParser, Parse_3) {
    int argc = 1;
    char arg0[32]{R"(D:\Users\sese\TestArgParser)"};
    char *argv[] = {arg0};

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));
}