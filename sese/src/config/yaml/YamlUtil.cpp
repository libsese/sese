#include "sese/config/yaml/YamlUtil.h"

#include <sstream>
#include <vector>

int sese::yaml::YamlUtil::getSpaceCount(const std::string &line) noexcept {
    int value = 0;
    for (decltype(auto) ch: line) {
        if (ch == ' ') {
            value += 1;
        } else {
            return value;
        }
    }
    return value;
}

std::tuple<int, std::string> sese::yaml::YamlUtil::getLine(sese::InputStream *input) noexcept {
    std::stringstream stream;
    char ch;
    while (true) {
        auto l = input->read(&ch, 1);
        if (l <= 0) {
            goto ret;
        }
        if (ch == '\r' || ch == '\n') {
            if (stream.str().length() > 0) {
                goto ret;
            } else {
                continue;
            }
        } else {
            stream << ch;
        }
    }

ret:
    auto str = stream.str();
    auto count = YamlUtil::getSpaceCount(str);
    return {count, str.substr(count)};
}

std::vector<std::string> sese::yaml::YamlUtil::tokenizer(const std::string &line) noexcept {
    std::vector<std::string> vector;
    std::stringstream stream;
    bool checkSpace = false;// 检查多余空格
    bool quot1 = false;     // 单引号 - 不转义
    bool quot2 = false;     // 双引号 - 转义
    bool tran = false;      // 是否转义
    for (decltype(auto) ch: line) {
        if (tran) {
            switch (ch) {
                case 'n':
                    stream << '\n';
                    break;
                case 'r':
                    stream << '\r';
                    break;
                case '\"':
                case '\'':
                case '\\':
                    stream << ch;
                    break;
                default:
                    stream << '\0';
                    break;
            }
            tran = false;
        } else if (ch == '-') {
            vector.emplace_back("-");
            checkSpace = true;
        } else if (ch == ':') {
            auto str = stream.str();
            if (!str.empty()) {
                vector.emplace_back(stream.str());
            }
            vector.emplace_back(":");
            checkSpace = true;
            stream.str("");
        } else if (ch == '\'') {
            if (quot1) {
                quot1 = false;
                vector.emplace_back(stream.str());
                stream.str("");
            } else {
                quot1 = true;
                checkSpace = false;
            }
        } else if (ch == '|') {
            vector.emplace_back("|");
        } else if (ch == '\"') {
            if (quot1) {
                stream << ch;
            } else if (quot2) {
                quot2 = false;
                vector.emplace_back(stream.str());
                stream.str("");
            } else {
                quot2 = true;
                checkSpace = false;
            }
        } else if (ch == '\\') {
            tran = true;
        } else {
            if (checkSpace) {
                if (ch == ' ') {
                    continue;
                } else {
                    checkSpace = false;
                    stream << ch;
                }
            } else {
                stream << ch;
            }
        }
    }

    auto str = stream.str();
    if (!str.empty()) {
        vector.emplace_back(str);
    }
    return vector;
}