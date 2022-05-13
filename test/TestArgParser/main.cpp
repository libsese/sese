#include "sese/ArgParser.h"
#include "sese/record/LogHelper.h"
#include "sese/Test.h"

sese::LogHelper helper("fARG_PARSER"); // NOLINT

int main(int32_t argc, char *argv[]) {
    auto args = std::make_unique<sese::ArgParser>();
    sese::Test::assert(helper, args->parse(argc, argv), -1);

    auto findValue = [&args](const std::string &key, const std::string &defaultValue) {
        helper.info("%s = %s",
                    key.c_str(),
                    args->getValueByKey(key, defaultValue).c_str());
    };

    findValue("a1", "undef");
    findValue("a2", "undef");
    findValue("a3", "undef");

    return 0;
}