#define SESE_C_LIKE_FORMAT

#include "sese/util/ArgParser.h"
#include "sese/record/Marco.h"
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

    SESE_INFO("full path: %s", args->getFullPath().c_str());
    SESE_INFO("current path: %s", args->getCurrentPath().c_str());
    SESE_INFO("exec file name: %s", args->getFileName().c_str());

    auto find_value = [&args](const std::string &key, const std::string &default_value) {
        SESE_INFO("%s = %s", key.c_str(), args->getValueByKey(key, default_value).c_str());
    };

    find_value("a1", "undef");
    find_value("a2", "undef");
    find_value("a3", "undef");
    find_value("a4", "undef");

    auto set = args->getKeyValSet();
    for (auto &item: set) {
        SESE_INFO("%s: %s", item.first.c_str(), item.second.c_str());
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
