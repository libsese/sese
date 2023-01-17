#include "sese/util/ArgParser.h"
#include "sese/record/LogHelper.h"
#include "gtest/gtest.h"

TEST(TestArgParser, _0) {
    sese::record::LogHelper log("ARGS");

    int argc = 4;
    char arg0[16] {"path_to_exec"};
    char arg1[16] {"a1=hello"};
    char arg2[16] {"a2=\"a sentence\""};
    char arg3[16] {"a3=123"};
    char *argv[] = {arg0, arg1, arg2, arg3};

    auto args = std::make_unique<sese::ArgParser>();
    EXPECT_TRUE(args->parse(argc, argv));

    auto findValue = [&args, &log](const std::string &key, const std::string &defaultValue) {
        log.info("%s = %s", key.c_str(), args->getValueByKey(key, defaultValue).c_str());
    };

    findValue("a1", "undef");
    findValue("a2", "undef");
    findValue("a3", "undef");
}