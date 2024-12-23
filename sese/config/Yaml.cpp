// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <cstdlib>
#include <sese/config/Yaml.h>
#include <sese/util/Util.h>

#include <array>

using sese::Value;
using sese::Yaml;

// GCOVR_EXCL_START

Value Yaml::parseBasic(const std::string &value) {
    if (strcmpDoNotCase(value.c_str(), "null") || value == "~" || value.empty()) {
        return {};
    }
    if (strcmpDoNotCase(value.c_str(), "true") ||
        strcmpDoNotCase(value.c_str(), "yes")) {
        return Value{true};
    }
    if (strcmpDoNotCase(value.c_str(), "false") || sese::strcmpDoNotCase(value.c_str(), "no")) {
        return Value{false};
    }
    if (value.find('.') != std::string::npos) {
        char *end;
        double d = std::strtod(value.c_str(), &end);
        if (*end == 0) {
            return Value{d};
        }
    } else {
        char *end;
        int64_t l = std::strtol(value.c_str(), &end, 10);
        if (*end == 0) {
            return Value{l};
        }
    }
    return Value{value.c_str()};
}

bool Yaml::parseObject(TokensQueue &tokens_queue, std::stack<std::pair<Value *, int>> &stack) {
    auto [object, count] = stack.top();
    auto &object_ref = object->getDict();
    stack.pop();
    // object does not have children
    if (tokens_queue.empty()) {
        return true;
    }
    // is the first element?
    if (count == -1) {
        count = std::get<0>(tokens_queue.front());
    }

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        auto current_count = std::get<0>(count_and_tokens);
        auto current_tokens = std::get<1>(count_and_tokens);

        if (count == current_count) {
            if (current_tokens.size() == 3) {
                if (current_tokens[1] != ":") {
                    return false;
                }
                // normal key-value
                const std::string &key = current_tokens[0];
                const std::string &value = current_tokens[2];
                object_ref.set(key, parseBasic(value));
                tokens_queue.pop();
            } else if (current_tokens.size() == 2) {
                // may be empty object or new object or new array
                auto current_count_and_tokens = tokens_queue.front();
                tokens_queue.pop();
                if (tokens_queue.empty()) {
                    // does not exist next line, and current element is null
                    object_ref.set(current_tokens[0], Value());
                } else {
                    // the next line is exist
                    auto next_count_and_tokens = tokens_queue.front();
                    auto next_count = std::get<0>(next_count_and_tokens);
                    auto next_tokens = std::get<1>(next_count_and_tokens);
                    if (next_count == count) {
                        // current element is null, and next element is belonged to this object
                        object_ref.set(current_tokens[0], Value());
                    } else if (next_count < count) {
                        // current element is null, and next element is not belong to this object
                        object_ref.set(current_tokens[0], Value());
                        return true;
                    } else if (next_count > count) {
                        // new array
                        if (next_tokens[0] == "-") {
                            auto value_object = object_ref.setRef(current_tokens[0], Value::list()).get();
                            stack.emplace(object, count);
                            stack.emplace(value_object, -1);
                            return true;
                        }
                        // new object
                        auto value_object = object_ref.setRef(current_tokens[0], Value::dict()).get();
                        stack.emplace(object, count);
                        stack.emplace(value_object, -1);
                        return true;
                    }
                }
            }
        } else if (count > current_count) {
            // end of current object
            return true;
        }
        // it should not happen
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return true;
}

bool Yaml::parseArray(TokensQueue &tokens_queue, std::stack<std::pair<Value *, int>> &stack) {
    auto [array, count] = stack.top();
    auto &array_ref = array->getList();
    stack.pop();
    // object does not have children
    if (tokens_queue.empty()) {
        return true;
    }

    // is the first element?
    if (count == -1) {
        count = std::get<0>(tokens_queue.front());
    }

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        auto &current_count = std::get<0>(count_and_tokens);
        auto &current_tokens = std::get<1>(count_and_tokens);

        if (count > current_count) {
            // end of current array
            return true;
        }
        // prefix '-' must be erased
        current_tokens.erase(current_tokens.begin());

        if (count == current_count) {
            // empty
            if (current_tokens.empty()) {
                array_ref.append(Value());
                tokens_queue.pop();
            }
            // basic value
            else if (current_tokens.size() == 1) {
                array_ref.append(parseBasic(current_tokens[0]));
                tokens_queue.pop();
            }
            // object
            else if ((current_tokens.size() == 2 || current_tokens.size() == 3) && current_tokens[1] == ":") {
                current_count += 2;
                auto value_object = array_ref.appendRef(Value::dict()).get();
                stack.emplace(array, count);
                stack.emplace(value_object, -1);
                return true;
            }
            // array
            else if (current_tokens[0] == "-") {
                current_count += 2;
                auto value_object = array_ref.appendRef(Value::list()).get();
                stack.emplace(array, count);
                stack.emplace(value_object, -1);
                return true;
            } else {
                return false;
            }
        }
        // it should not happen
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return true;
}

int Yaml::getSpaceCount(const std::string &line) noexcept {
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

std::tuple<int, std::string> Yaml::getLine(InputStream *input) noexcept {
    text::StringBuilder builder(1024);
    char ch;
    while (true) {
        auto l = input->read(&ch, 1);
        if (l <= 0) {
            goto ret;
        }
        if (ch == '\r' || ch == '\n') {
            if (!builder.empty()) {
                goto ret;
            } else {
                continue;
            }
        } else {
            builder << ch;
        }
    }

ret:
    auto str = builder.toString();
    auto count = Yaml::getSpaceCount(str);
    return {count, str.substr(count)};
}

std::vector<std::string> Yaml::tokenizer(const std::string &line) noexcept {
    std::vector<std::string> vector;
    text::StringBuilder builder(1024);
    bool is_str = false; // Is a string?
    bool quot1 = false;  // Is single quotes?
    bool quot2 = false;  // Is double quotes?
    bool tran = false;   // Is escape characters?
    for (decltype(auto) ch: line) {
        if (quot1) {
            if (ch == '\'') {
                is_str = false;
                quot1 = false;
                vector.emplace_back(builder.toString());
                builder.clear();
            } else {
                builder << ch;
            }
        } else if (quot2) {
            if (tran) {
                // Do not automatically handle escape characters
                // switch (ch) {
                //     case 'n':
                //         stream << '\n';
                //         break;
                //     case 'r':
                //         stream << '\r';
                //         break;
                //     case '"':
                //     case '\'':
                //     case '\\':
                //     default:
                //         stream << ch;
                //         break;
                // }
                builder << '\\';
                builder << ch;
                tran = false;
            } else if (ch == '\"') {
                is_str = false;
                quot2 = false;
                vector.emplace_back(builder.toString());
                builder.clear();
            } else if (ch == '\\') {
                tran = true;
            } else {
                builder << ch;
            }
        } else if (is_str) {
            if (ch == ':') {
                is_str = false;
                vector.emplace_back(builder.toString());
                vector.emplace_back(":");
                builder.clear();
            } else {
                builder << ch;
            }
        } else if (ch == '\'') {
            quot1 = true;
            is_str = true;
        } else if (ch == '\"') {
            quot2 = true;
            is_str = true;
        } else if (ch == ':') {
            vector.emplace_back(":");
        } else if (ch == '-') {
            vector.emplace_back("-");
        } else if (ch == ' ') {
            continue;
        } else {
            builder << ch;
            is_str = true;
        }
    }

    auto str = builder.toString();
    if (!str.empty()) {
        vector.emplace_back(str);
    }
    return vector;
}

constexpr auto GET_SPACE_ARRAY = []() {
    std::array<char, 1024> array{};
    for (decltype(auto) item: array) {
        item = ' ';
    }
    return array;
};

void Yaml::writeSpace(size_t count, OutputStream *output) noexcept {
    const auto BUFFER = GET_SPACE_ARRAY();
    const auto SIZE = std::min<size_t>(1024, count);
    output->write(BUFFER.data(), SIZE);
}

Value Yaml::parse(InputStream *input) {
    TokensQueue tokens_queue;
    while (true) {
        decltype(auto) line = getLine(input);
        if (std::get<1>(line).empty()) {
            break;
        }
        auto tokens = tokenizer(std::get<1>(line));
        tokens_queue.emplace(std::get<0>(line), tokens);
    }

    if (tokens_queue.empty())
        return {};
    decltype(auto) top = tokens_queue.front();
    decltype(auto) top_tokens = std::get<1>(top);

    Value root_value;
    if (top_tokens.empty())
        return {};
    if (top_tokens[0] == "-") {
        // The root element is an array
        root_value = Value::list();
    } else {
        // The root element is an object
        root_value = Value::dict();
    }

    std::stack<std::pair<Value *, int>> stack;
    stack.emplace(&root_value, -1);

    while (!stack.empty()) {
        bool rt = false;
        auto [current_value, _] = stack.top();
        if (current_value->isDict()) {
            rt = parseObject(tokens_queue, stack);
        } else if (current_value->isList()) {
            rt = parseArray(tokens_queue, stack);
        }
        if (!rt) {
            return {};
        }
    }

    return root_value;
}

void Yaml::streamifyObject(OutputStream *output, const Value::Dict &dict, size_t level) {
    for (const auto &[fst, snd]: dict) {
        if (snd->getType() == Value::Type::DICT) {
            if (auto &&sub = snd->getDict(); !sub.empty()) {
                writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (snd->getType() == Value::Type::LIST) {
            if (auto &&sub = snd->getList(); !sub.empty()) {
                writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                streamifyArray(output, sub, level + 1);
            }
        } else {
            const auto SUB = snd;
            writeSpace(level * 2, output);
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

void Yaml::streamifyArray(OutputStream *output, const Value::List &list, size_t level) {
    auto count = 0;
    for (decltype(auto) item: list) {
        if (item->getType() == Value::Type::DICT) {
            if (auto &&sub = item->getDict(); !sub.empty()) {
                auto name = "element_" + std::to_string(count);
                count += 1;
                Yaml::writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                streamifyObject(output, sub, level + 1);
            }
        } else if (item->getType() == Value::Type::LIST) {
            auto &&sub = item->getList();
            if (!sub.empty()) {
                auto name = "element_" + std::to_string(count);
                count += 1;
                Yaml::writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                streamifyArray(output, sub, level + 1);
            }
        } else {
            auto &&sub = item;
            Yaml::writeSpace(level * 2, output);
            output->write("- ", 2);
            if (sub->isNull()) {
                output->write("~", 1);
            } else if (sub->isBool()) {
                auto data = sub->getBool() ? "true" : "false";
                output->write(data, strlen(data));
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