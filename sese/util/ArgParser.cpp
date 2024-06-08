#include "sese/util/ArgParser.h"
#include "sese/util/Util.h"
#include "sese/text/StringBuilder.h"

#include <algorithm>
#include <cstring>

bool sese::ArgParser::parse(int32_t argc, char **argv) noexcept {
#ifdef SESE_PLATFORM_WINDOWS
    std::string s(argv[0]);
    std::replace(s.begin(), s.end(), '\\', '/');
#endif

    fullPath = s;
    auto index = fullPath.find_last_of('/');
    currentPath = fullPath.substr(0, index);
    fileName = fullPath.substr(index + 1, fullPath.size() - index - 1);

    if (argc != 1) {
        for (int i = 1; i < argc; i++) {
            auto res = text::StringBuilder::split(argv[i], "=");
            if (res.size() == 1) {
                this->keyValSet[res[0]] = "";
            } else if (res.size() == 2) {
                this->keyValSet[res[0]] = res[1];
            } else {
                return false;
            }
        }
    }
    return true;
}

bool sese::ArgParser::exist(const std::string &key) const noexcept {
    return this->keyValSet.find(key) != this->keyValSet.end();
}

const std::map<std::string, std::string> &sese::ArgParser::getKeyValSet() const noexcept {
    return this->keyValSet;
}

const std::string &sese::ArgParser::getValueByKey(const std::string &key,
                                                  const std::string &default_value) const noexcept {
    for (const auto &[k, v]: keyValSet) {
        if (k == key) {
            return v;
        }
    }
    return default_value;
}
