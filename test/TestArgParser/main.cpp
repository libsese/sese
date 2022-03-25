#include "ArgParser.h"
#include "Util.h"

#define FILTER_TEST_ARG_PARSER "fARG_PARSER"

int main(int32_t argc, char *argv[]) {
    auto args = std::make_unique<sese::ArgParser>(argc, argv);

    auto findValue = [&args](const std::string &key, const std::string &defaultValue) {
        ROOT_INFO(FILTER_TEST_ARG_PARSER,
                  "%s = %s",
                  key.c_str(),
                  args->getValueByKey(key, defaultValue).c_str())
    };

    findValue("a1", "undef");
    findValue("a2", "undef");
    findValue("a3", "undef");

    return 0;
}