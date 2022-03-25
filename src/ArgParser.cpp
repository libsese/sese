#include "ArgParser.h"
#include "IllegalArgumentException.h"
#include "Util.h"
#include <cstring>

sese::ArgParser::ArgParser(int32_t argc, char **argv) {
    if (argc != 1) {
        for (int i = 1; i < argc; i++) {
            auto pos = findFirstAt(argv[i], '=');
            if (pos > 0) {
                if (strlen(argv[i]) - 1 == pos) {
                    continue;
                } else {
                    argv[i][pos] = '\0';
                    this->keyValSet.insert(std::pair<std::string, std::string>(argv[i], {&argv[i][pos + 1]}));
                }
            } else {
                throw IllegalArgumentException();
            }
        }
    }
}

const std::map<std::string, std::string> &sese::ArgParser::getKeyValSet() const noexcept {
    return this->keyValSet;
}

const std::string &sese::ArgParser::getValueByKey(const std::string &key, const std::string &defaultValue) const noexcept {
    for (const auto &itor: keyValSet) {
        if (itor.first == key) {
            return itor.second;
        }
    }
    return defaultValue;
}
