#include "sese/config/yaml/YamlUtil.h"

#include <sstream>

sese::yaml::Data::Ptr sese::yaml::YamlUtil::deserialize(sese::InputStream *input, size_t level) noexcept {
    TokensQueue tokensQueue;
    while (true) {
        decltype(auto) line = sese::yaml::YamlUtil::getLine(input);
        if (std::get<1>(line).length() == 0) {
            break;
        }
        auto tokens = sese::yaml::YamlUtil::tokenizer(std::get<1>(line));
        tokensQueue.emplace(std::get<0>(line), tokens);
    }

    decltype(auto) top = tokensQueue.front();
    decltype(auto) topTokens = std::get<1>(top);
    if (topTokens.empty()) return nullptr;
    if (topTokens[0] == "-") {
        // 根元素是数组
        return createArray(tokensQueue, level);
    } else {
        // 根元素是对象
        return createObject(tokensQueue, level);
    }
}

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
    bool isStr = false;// 是否是字符串
    bool quot1 = false;// 单引号 - 不转义
    bool quot2 = false;// 双引号 - 转义
    bool tran = false; // 是否转义
    for (decltype(auto) ch: line) {
        if (quot1) {
            if (ch == '\'') {
                isStr = false;
                quot1 = false;
                vector.emplace_back(stream.str());
                stream.str("");
            } else {
                stream << ch;
            }
        } else if (quot2) {
            if (tran) {
                switch (ch) {
                    case 'n':
                        stream << '\n';
                        break;
                    case 'r':
                        stream << '\r';
                        break;
                    case '"':
                    case '\'':
                    case '\\':
                        stream << ch;
                        break;
                    default:
                        stream << '\0';
                        break;
                }
                tran = false;
            } else if (ch == '\"') {
                isStr = false;
                quot2 = false;
                vector.emplace_back(stream.str());
                stream.str("");
            } else if (ch == '\\') {
                tran = true;
            } else {
                stream << ch;
            }
        } else if (isStr) {
            if (ch == ':') {
                isStr = false;
                vector.emplace_back(stream.str());
                vector.emplace_back(":");
                stream.str("");
            } else {
                stream << ch;
            }
        } else if (ch == '\'') {
            quot1 = true;
            isStr = true;
        } else if (ch == '\"') {
            quot2 = true;
            isStr = true;
        } else if (ch == ':') {
            vector.emplace_back(":");
        } else if (ch == '-') {
            vector.emplace_back("-");
        } else if (ch == '|') {
            vector.emplace_back("|");
        } else if (ch == ' ') {
            continue;
        } else {
            stream << ch;
            isStr = true;
        }
    }

    auto str = stream.str();
    if (!str.empty()) {
        vector.emplace_back(str);
    }
    return vector;
}

sese::yaml::ObjectData::Ptr
sese::yaml::YamlUtil::createObject(
        sese::yaml::YamlUtil::TokensQueue &tokensQueue,
        size_t level
) noexcept {
    if (level == 0) return nullptr;

    auto result = std::make_shared<ObjectData>();
    // 对象无子元素
    if (tokensQueue.empty()) return result;
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 3) {
                // 普通键值对
                std::string key = currentTokens[0];
                std::string value = currentTokens[2];
                auto valueObject = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    valueObject->setNull(true);
                } else {
                    valueObject->setDataAs<std::string>(value);
                }
                result->set(key, valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                // 可能是空对象，也可能是新对象或新数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行且当前元素为 null
                    auto valueObject = std::make_shared<BasicData>();
                    valueObject->setNull(true);
                    result->set(currentTokens[0], valueObject);
                } else {
                    // 存在下一行
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount == count) {
                        // 当前行元素是 null
                        // 下一个元素依然是本对象所属
                        auto valueObject = std::make_shared<BasicData>();
                        valueObject->setNull(true);
                        result->set(currentTokens[0], valueObject);
                    } else if (nextCount < count) {
                        // 当前行元素是 null
                        // 下一个元素不是本对象所属
                        auto valueObject = std::make_shared<BasicData>();
                        valueObject->setNull(true);
                        result->set(currentTokens[0], valueObject);
                        return result;
                    } else if (nextCount > count) {
                        // 新的数组
                        if (nextTokens[0] == "-") {
                            auto valueObject = createArray(tokensQueue, level - 1);
                            if (valueObject) {
                                result->set(currentTokens[0], valueObject);
                            }
                        }
                        // 新的对象
                        else {
                            auto valueObject = createObject(tokensQueue, level - 1);
                            if (valueObject) {
                                result->set(currentTokens[0], valueObject);
                            }
                        }
                    }
                }
            }
        } else if (count > currentCount) {
            // 当前对象结束
            return result;
        } else {
            // 错误
            tokensQueue.pop();
            continue;
        }
    }
    return result;
}

sese::yaml::ArrayData::Ptr
sese::yaml::YamlUtil::createArray(
        sese::yaml::YamlUtil::TokensQueue &tokensQueue,
        size_t level
) noexcept {
    if (level == 0) return nullptr;

    auto result = std::make_shared<ArrayData>();
    // 对象无子元素
    if (tokensQueue.empty()) return result;
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 1) {
                auto valueObject = std::make_shared<BasicData>();
                valueObject->setNull(true);
                result->push(valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                std::string value = currentTokens[1];
                auto valueObject = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    valueObject->setNull(true);
                } else {
                    valueObject->setDataAs<std::string>(value);
                }
                result->push(valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 3) {
                // 新的对象或数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行，跳过该对象
                    return result;
                } else {
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount > count) {
                        if (nextTokens[0] == "=") {
                            auto valueObject = createArray(tokensQueue, level - 1);
                            if (valueObject) {
                                result->push(valueObject);
                            }
                        } else {
                            auto valueObject = createObject(tokensQueue, level - 1);
                            if (valueObject) {
                                result->push(valueObject);
                            }
                        }
                    } else {
                        tokensQueue.pop();
                        continue;
                    }
                }
            }
        } else if (count > currentCount) {
            // 当前数组结束
            return result;
        } else {
            // 错误
            tokensQueue.pop();
            continue;
        }
    }
    return result;
}