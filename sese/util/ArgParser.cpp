#include "sese/util/ArgParser.h"
#include "sese/util/Util.h"
#include "sese/text/StringBuilder.h"

#include <cstring>

bool sese::ArgParser::parse(int32_t argc, char **argv) noexcept {
    char *p = argv[0];
    while(*p != 0) {
        if (*p == '\\') {
            *p = '/';
        }
        p++;
    }

    fullPath = argv[0];
    auto index = fullPath.find_last_of('/');
    currentPath = fullPath.substr(0, index);
    fileName = fullPath.substr(index + 1, fullPath.size() - index - 1);

    if (argc != 1) {
        for (int i = 1; i < argc; i++) {
            // auto pos = findFirstAt(argv[i], '=');
            // if (pos > 0) {
            //     if (strlen(argv[i]) - 1 == pos) {
            //         continue;
            //     } else {
            //         argv[i][pos] = '\0';
            //         this->keyValSet.insert(std::pair<std::string, std::string>(argv[i], {&argv[i][pos + 1]}));
            //     }
            // } else {
            //     return false;
            // }
            auto res = sese::text::StringBuilder::split(argv[i], "=");
            if (res.size() != 2) return false;
            this->keyValSet.insert(std::pair<std::string, std::string>(res[0], res[1]));
        }
    }
    return true;
}

const std::map<std::string, std::string> &sese::ArgParser::getKeyValSet() const noexcept {
    return this->keyValSet;
}

const std::string &sese::ArgParser::getValueByKey(const std::string &key, const std::string &defaultValue) const noexcept {
    for (const auto &iterator: keyValSet) {
        if (iterator.first == key) {
            return iterator.second;
        }
    }
    return defaultValue;
}
