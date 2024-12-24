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

bool Yaml::parseObject(TokensQueue &tokens_queue, ParseStack &stack) {
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

bool Yaml::parseArray(TokensQueue &tokens_queue, ParseStack &stack) {
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

#define CONST_WRITE(stream, const_string)                                          \
    if (stream->write(const_string, strlen(const_string)) != strlen(const_string)) \
    return false

#define STRING_WRITE(stream, string)                                  \
    if (stream->write(string.data(), string.size()) != string.size()) \
    return false

bool Yaml::writeSpace(OutputStream *output, size_t count) noexcept {
    char buffer[1024]{};
    memset(buffer, ' ', sizeof(buffer));
    count = count % sizeof(buffer);
    auto times = count / sizeof(buffer);
    for (auto i = 0; i < times; ++i) {
        if (output->write(buffer, sizeof(buffer)) != sizeof(buffer)) {
            return false;
        }
    }
    if (output->write(buffer, count) != count) {
        return false;
    }
    return true;
}

bool Yaml::streamifyObject(
    OutputStream *output,
    StreamifyStack &stack,
    StreamifyIterStack &map_iter_stack
) {
    auto iter = map_iter_stack.top();
    map_iter_stack.pop();
    auto &[object, count, level, prefix] = stack.top();
    auto &object_ref = object->getDict();
    stack.pop();
    while (iter != object_ref.end()) {
        auto name = iter->first;
        auto value = iter->second;
        ++iter;
        count += 1;
        if (count != 1 || prefix) {
            if (!writeSpace(output, level * 2)) {
                return false;
            }
        }
        STRING_WRITE(output, name);
        CONST_WRITE(output, ": ");
        if (value->isDict()) {
            CONST_WRITE(output, "\n");
            stack.emplace(object, count, level, true);
            stack.emplace(value.get(), 0, level + 1, true);
            map_iter_stack.emplace(iter);
            map_iter_stack.emplace(value->getDict().begin());
            return true;
        }
        if (value->isList()) {
            CONST_WRITE(output, "\n");
            stack.emplace(object, count, level, true);
            stack.emplace(value.get(), 0, level + 1, true);
            map_iter_stack.emplace(iter);
            return true;
        }
        if (!streamifyBasic(output, value)) {
            return false;
        }
    }
    return true;
}

bool Yaml::streamifyArray(
    OutputStream *output,
    StreamifyStack &stack,
    StreamifyIterStack &map_iter_stack
) {
    auto &[array, count, level, prefix] = stack.top();
    auto &array_ref = array->getList();
    stack.pop();
    while (count < array_ref.size()) {
        auto value = array_ref[count];
        count += 1;
        if (count != 1 || prefix) {
            if (!writeSpace(output, level * 2)) {
                return false;
            }
        }
        if (value->isDict()) {
            CONST_WRITE(output, "- ");
            stack.emplace(array, count, level, true);
            stack.emplace(value.get(), 0, level + 1, false);
            map_iter_stack.emplace(value->getDict().begin());
            return true;
        }
        if (value->isList()) {
            CONST_WRITE(output, "- ");
            stack.emplace(array, count, level, true);
            stack.emplace(value.get(), 0, level + 1, false);
            return true;
        }
        CONST_WRITE(output, "- ");
        if (!streamifyBasic(output, value)) {
            return false;
        }
    }
    return true;
}

bool Yaml::streamifyBasic(OutputStream *output, const std::shared_ptr<Value> &value) {
    if (value->isNull()) {
        CONST_WRITE(output, "~");
    } else if (value->isBool()) {
        decltype(auto) data = value->getBool() ? "true" : "false";
        CONST_WRITE(output, data);
    } else if (value->isInt() || value->isDouble()) {
        decltype(auto) data = value->toStringBasic();
        STRING_WRITE(output, data);
    } else {
        // value->isString()
        decltype(auto) data = value->getString();
        STRING_WRITE(output, data);
    }
    CONST_WRITE(output, "\n");
    return true;
}

bool Yaml::streamify(OutputStream *output, Value &value) {
    StreamifyStack stack;
    StreamifyIterStack map_iter_stack;
    stack.emplace(&value, 0, 0, true);
    if (value.isDict()) {
        map_iter_stack.emplace(value.getDict().begin());
    }

    while (!stack.empty()) {
        auto &[current_value, count, level, prefix] = stack.top();
        bool rt = false;
        if (current_value->isDict()) {
            rt = streamifyObject(output, stack, map_iter_stack);
        } else if (current_value->isList()) {
            rt = streamifyArray(output, stack, map_iter_stack);
        } else {
            return false;
        }
        if (!rt) {
            return false;
        }
    }
    return true;
}

// GCOVR_EXCL_STOP