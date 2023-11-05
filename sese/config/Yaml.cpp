#include <sese/config/Yaml.h>
#include <sese/util/Util.h>

using sese::Value;
using sese::Yaml;

// GCOVR_EXCL_START

Value Yaml::parseObject(sese::Yaml::TokensQueue &tokensQueue, size_t level) {
    if (level == 0) return {};

    auto result = Value::Dict();
    // 对象无子元素
    if (tokensQueue.empty()) return Value(std::move(result));
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 3) {
                if (currentTokens[1] != ":") {
                    return {};
                }
                // 普通键值对
                std::string key = currentTokens[0];
                std::string value = currentTokens[2];
                if (sese::strcmpDoNotCase(value.c_str(), "null") || value == "~") {
                    result.set(key, Value());
                } else {
                    result.set(key, std::move(value));
                }
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                // 可能是空对象，也可能是新对象或新数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行且当前元素为 null
                    result.set(currentTokens[0], Value());
                } else {
                    // 存在下一行
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount == count) {
                        // 当前行元素是 null
                        // 下一个元素依然是本对象所属
                        result.set(currentTokens[0], Value());
                    } else if (nextCount < count) {
                        // 当前行元素是 null
                        // 下一个元素不是本对象所属
                        result.set(currentTokens[0], Value());
                        return Value(std::move(result));
                    } else if (nextCount > count) {
                        // 新的数组
                        if (nextTokens[0] == "-") {
                            auto valueObject = parseArray(tokensQueue, level - 1);
                            if (valueObject.isList()) {
                                result.set(currentTokens[0], std::move(valueObject));
                            } else {
                                return {};
                            }
                        }
                        // 新的对象
                        else {
                            auto valueObject = parseObject(tokensQueue, level - 1);
                            if (valueObject.isDict()) {
                                result.set(currentTokens[0], std::move(valueObject));
                            } else {
                                return {};
                            }
                        }
                    }
                }
            }
        } else if (count > currentCount) {
            // 当前对象结束
            return Value(std::move(result));
        }
        // 错误 - 这几乎不可能发生
        // GCOVR_EXCL_START
        else {
            tokensQueue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return Value(std::move(result));
}

Value Yaml::parseArray(Yaml::TokensQueue &tokensQueue, size_t level) {
    if (level == 0) return {};

    auto result = Value::List();
    // 对象无子元素
    if (tokensQueue.empty()) return Value(std::move(result));
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 1) {
                result.append(Value());
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                std::string value = currentTokens[1];
                // auto valueObject = std::make_shared<BasicData>();
                if (sese::strcmpDoNotCase(value.c_str(), "null") || value == "~") {
                    ;
                    result.append(Value());
                } else {
                    result.append(std::move(value));
                }
                // result->push(valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 3) {
                // 新的对象或数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行，跳过该对象
                    return Value(std::move(result));
                } else {
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount > count) {
                        if (nextTokens[0] == "-") {
                            auto valueObject = parseArray(tokensQueue, level - 1);
                            if (valueObject.isList()) {
                                result.append(std::move(valueObject));
                            } else {
                                return {};
                            }
                        } else {
                            auto valueObject = parseObject(tokensQueue, level - 1);
                            if (valueObject.isDict()) {
                                result.append(std::move(valueObject));
                            } else {
                                return {};
                            }
                        }
                    }
                    // 错误 - 这几乎不会发生
                    // GCOVR_EXCL_START
                    else {
                        tokensQueue.pop();
                        continue;
                    }
                    // GCOVR_EXCL_STOP
                }
            }
        } else if (count > currentCount) {
            // 当前数组结束
            return Value(std::move(result));
        }
        // 错误 - 这几乎不会发生
        // GCOVR_EXCL_START
        else {
            tokensQueue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return Value(std::move(result));
}

#include <sese/config/yaml/YamlUtil.h>

Value Yaml::parse(sese::io::InputStream *input, size_t level) {
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
    if (topTokens.empty()) return {};
    if (topTokens[0] == "-") {
        // 根元素是数组
        return parseArray(tokensQueue, level);
    } else {
        // 根元素是对象
        return parseObject(tokensQueue, level);
    }
}

void Yaml::streamifyObject(io::OutputStream *output, const Value::Dict &dict, size_t level) {
    for (decltype(auto) item: dict) {
        if (item.second->getType() == Value::Type::Dict) {
            auto &&sub = item.second->getDict();
            if (!sub.empty()) {
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write(item.first.c_str(), item.first.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (item.second->getType() == Value::Type::List) {
            auto &&sub = item.second->getList();
            if (!sub.empty()) {
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write(item.first.c_str(), item.first.length());
                output->write(":\n", 2);
                streamifyArray(output, sub, level + 1);
            }
        } else {
            auto sub = item.second;
            sese::yaml::YamlUtil::writeSpace(level * 2, output);
            output->write(item.first.c_str(), item.first.length());
            output->write(": ", 2);
            if (sub->isNull()) {
                output->write("~", 1);
            } else if (sub->isBool()) {
                auto data = sub->getBool() ? "true" : "false";
                output->write(data, std::strlen(data));
            } else if (sub->isInt() || sub->isDouble()) {
                auto data = sub->toString();
                output->write(data.c_str(), data.length());
            } else if (sub->isString()) {
                auto &&data = sub->getString();
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}

void Yaml::streamifyArray(io::OutputStream *output, const Value::List &list, size_t level) {
    auto count = 0;
    for (decltype(auto) item: list) {
        if (item.getType() == Value::Type::Dict) {
            auto &&sub = item.getDict();
            if (!sub.empty()) {
                auto name = "element_" + std::to_string(count);
                count += 1;
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (item.getType() == Value::Type::List) {
            auto &&sub = item.getList();
            if (!sub.empty()) {
                auto name = "element_" + std::to_string(count);
                count += 1;
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                streamifyArray(output, sub, level + 1);
            }
        } else {
            auto &&sub = item;
            sese::yaml::YamlUtil::writeSpace(level * 2, output);
            output->write("- ", 2);
            if (sub.isNull()) {
                output->write("~", 1);
            } else if (sub.isBool()) {
                auto data = sub.getBool() ? "true" : "false";
                output->write(data, std::strlen(data));
            } else if (sub.isInt() || sub.isDouble()) {
                auto data = sub.toString();
                output->write(data.c_str(), data.length());
            } else if (sub.isString()) {
                auto &&data = sub.getString();
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}

void Yaml::streamify(io::OutputStream *output, const Value &value) {
    if (value.getType() == Value::Type::Dict) {
        auto &&sub = value.getDict();
        streamifyObject(output, sub, 0);
    } else if (value.getType() == Value::Type::List) {
        auto &&sub = value.getList();
        streamifyArray(output, sub, 0);
    } else {
        return;
    }
}

// GCOVR_EXCL_STOP