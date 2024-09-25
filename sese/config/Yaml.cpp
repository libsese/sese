#include <sese/config/Yaml.h>
#include <sese/util/Util.h>

using sese::Value;
using sese::Yaml;

// GCOVR_EXCL_START

Value Yaml::parseObject(sese::Yaml::TokensQueue &tokens_queue, size_t level) {
    if (level == 0) return {};

    auto result = Value::Dict();
    // 对象无子元素
    if (tokens_queue.empty()) return Value(std::move(result));
    int count = std::get<0>(tokens_queue.front());

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        auto current_count = std::get<0>(count_and_tokens);
        auto current_tokens = std::get<1>(count_and_tokens);

        if (count == current_count) {
            if (current_tokens.size() == 3) {
                if (current_tokens[1] != ":") {
                    return {};
                }
                // 普通键值对
                std::string key = current_tokens[0];
                std::string value = current_tokens[2];
                if (sese::strcmpDoNotCase(value.c_str(), "null") || value == "~") {
                    result.set(key, Value());
                } else {
                    result.set(key, std::move(value));
                }
                tokens_queue.pop();
            } else if (current_tokens.size() == 2) {
                // 可能是空对象，也可能是新对象或新数组
                auto current_count_and_tokens = tokens_queue.front();
                tokens_queue.pop();
                if (tokens_queue.empty()) {
                    // 不存在下一行且当前元素为 null
                    result.set(current_tokens[0], Value());
                } else {
                    // 存在下一行
                    auto next_count_and_tokens = tokens_queue.front();
                    auto next_count = std::get<0>(next_count_and_tokens);
                    auto next_tokens = std::get<1>(next_count_and_tokens);
                    if (next_count == count) {
                        // 当前行元素是 null
                        // 下一个元素依然是本对象所属
                        result.set(current_tokens[0], Value());
                    } else if (next_count < count) {
                        // 当前行元素是 null
                        // 下一个元素不是本对象所属
                        result.set(current_tokens[0], Value());
                        return Value(std::move(result));
                    } else if (next_count > count) {
                        // 新的数组
                        if (next_tokens[0] == "-") {
                            if (auto value_object = parseArray(tokens_queue, level - 1); value_object.isList()) {
                                result.set(current_tokens[0], std::move(value_object));
                            } else {
                                return {};
                            }
                        }
                        // 新的对象
                        else {
                            if (auto value_object = parseObject(tokens_queue, level - 1); value_object.isDict()) {
                                result.set(current_tokens[0], std::move(value_object));
                            } else {
                                return {};
                            }
                        }
                    }
                }
            }
        } else if (count > current_count) {
            // 当前对象结束
            return Value(std::move(result));
        }
        // 错误 - 这几乎不可能发生
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return Value(std::move(result));
}

Value Yaml::parseArray(Yaml::TokensQueue &tokens_queue, size_t level) {
    if (level == 0) return {};

    auto result = Value::List();
    // 对象无子元素
    if (tokens_queue.empty()) return Value(std::move(result));
    int count = std::get<0>(tokens_queue.front());

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        auto current_count = std::get<0>(count_and_tokens);
        auto current_tokens = std::get<1>(count_and_tokens);

        if (count == current_count) {
            if (current_tokens.size() == 1) {
                result.append(Value());
                tokens_queue.pop();
            }
            else if (current_tokens.size() == 2) {
                std::string value = current_tokens[1];
                // auto valueObject = std::make_shared<BasicData>();
                if (sese::strcmpDoNotCase(value.c_str(), "null") || value == "~") {
                    result.append(Value());
                } else {
                    result.append(std::move(value));
                }
                // result->push(valueObject);
                tokens_queue.pop();
            }
            else if (current_tokens.size() == 3) {
                // 新的对象或数组
                auto current_count_and_tokens = tokens_queue.front();
                tokens_queue.pop();
                if (tokens_queue.empty()) {
                    // 不存在下一行，跳过该对象
                    return Value(std::move(result));
                } else {
                    auto next_count_and_tokens = tokens_queue.front();
                    auto next_count = std::get<0>(next_count_and_tokens);
                    auto next_tokens = std::get<1>(next_count_and_tokens);
                    if (next_count > count) {
                        if (next_tokens[0] == "-") {
                            if (auto value_object = parseArray(tokens_queue, level - 1); value_object.isList()) {
                                result.append(std::move(value_object));
                            } else {
                                return {};
                            }
                        } else {
                            if (auto value_object = parseObject(tokens_queue, level - 1); value_object.isDict()) {
                                result.append(std::move(value_object));
                            } else {
                                return {};
                            }
                        }
                    }
                    // 错误 - 这几乎不会发生
                    // GCOVR_EXCL_START
                    else {
                        tokens_queue.pop();
                        continue;
                    }
                    // GCOVR_EXCL_STOP
                }
            }
            else if (current_tokens.size() == 4 &&
                     current_tokens[0] == "-" &&
                     current_tokens[2] == ":") {
                result.append(Value::Dict().set(current_tokens[1], std::move(current_tokens[3])));
                tokens_queue.pop();
            }
            else {
                return {};
            }
        } else if (count > current_count) {
            // 当前数组结束
            return Value(std::move(result));
        }
        // 错误 - 这几乎不会发生
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return Value(std::move(result));
}

#include <sese/config/yaml/YamlUtil.h>

Value Yaml::parse(sese::io::InputStream *input, size_t level) {
    TokensQueue tokens_queue;
    while (true) {
        decltype(auto) line = sese::yaml::YamlUtil::getLine(input);
        if (std::get<1>(line).empty()) {
            break;
        }
        auto tokens = sese::yaml::YamlUtil::tokenizer(std::get<1>(line));
        tokens_queue.emplace(std::get<0>(line), tokens);
    }

    decltype(auto) top = tokens_queue.front();
    decltype(auto) top_tokens = std::get<1>(top);
    if (top_tokens.empty()) return {};
    if (top_tokens[0] == "-") {
        // 根元素是数组
        return parseArray(tokens_queue, level);
    } else {
        // 根元素是对象
        return parseObject(tokens_queue, level);
    }
}

void Yaml::streamifyObject(io::OutputStream *output, const Value::Dict &dict, size_t level) {
    for (const auto &[fst, snd]: dict) {
        if (snd->getType() == Value::Type::DICT) {
            if (auto &&sub = snd->getDict(); !sub.empty()) {
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (snd->getType() == Value::Type::LIST) {
            if (auto &&sub = snd->getList(); !sub.empty()) {
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                streamifyArray(output, sub, level + 1);
            }
        } else {
            const auto SUB = snd;
            sese::yaml::YamlUtil::writeSpace(level * 2, output);
            output->write(fst.c_str(), fst.length());
            output->write(": ", 2);
            if (SUB->isNull()) {
                output->write("~", 1);
            } else if (SUB->isBool()) {
                const auto DATA = SUB->getBool() ? "true" : "false";
                output->write(DATA, strlen(DATA));
            } else if (SUB->isInt() || SUB->isDouble()) {
                auto data = SUB->toString();
                output->write(data.c_str(), data.length());
            } else if (SUB->isString()) {
                auto &&data = SUB->getString();
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}

void Yaml::streamifyArray(io::OutputStream *output, const Value::List &list, size_t level) {
    auto count = 0;
    for (decltype(auto) item: list) {
        if (item.getType() == Value::Type::DICT) {
            if (auto &&sub = item.getDict(); !sub.empty()) {
                auto name = "element_" + std::to_string(count);
                count += 1;
                sese::yaml::YamlUtil::writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (item.getType() == Value::Type::LIST) {
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
                output->write(data, strlen(data));
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
    if (value.getType() == Value::Type::DICT) {
        auto &&sub = value.getDict();
        streamifyObject(output, sub, 0);
    } else if (value.getType() == Value::Type::LIST) {
        auto &&sub = value.getList();
        streamifyArray(output, sub, 0);
    } else {
        return;
    }
}

// GCOVR_EXCL_STOP