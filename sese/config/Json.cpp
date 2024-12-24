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

#include <sese/config/Json.h>

#include <cassert>

using sese::Json;
using sese::Value;

// GCOVR_EXCL_START

/// Checks if a token is a JSON keyword
/// \param str The token
/// \return The result
inline bool isKeyword(const char *str) {
    if (str[0] == '}' || str[0] == ']' || str[0] == ':' || str[0] == ',') {
        return true;
    }
    return false;
}

bool Json::tokenizer(io::InputStream *input_stream, Tokens &tokens) noexcept {
    char ch;
    text::StringBuilder builder;

    int64_t len;
    while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
        switch (ch) {
            case '{':
            case '}':
            case '[':
            case ']':
            case ',':
            case ':':
                // General token
                tokens.push({ch});
                break;
            case '\"':
                builder.append("\"");
                // String token, need to handle escape characters
                while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
                    // Is escape character
                    if (ch == '\\') {
                        if (input_stream->read(&ch, 1 * sizeof(char)) == 0) {
                            // String is incomplete
                            return false;
                        }
                        builder << '\\';
                        builder << ch;
                    } else if (ch == '\"') {
                        builder.append("\"");
                        tokens.push(builder.toString());
                        builder.clear();
                        break;
                    } else {
                        builder.append(ch);
                    }
                }
                break;
            default:
                // Normal token
                if (sese::isSpace(ch))
                    break;
                builder.append(ch);
                while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
                    // fix: There need to be more keywords
                    if (isKeyword(&ch)) {
                        tokens.push(builder.toString());
                        builder.clear();
                        tokens.push({ch});
                        break;
                    }
                    if (sese::isSpace(ch)) {
                        tokens.push(builder.toString());
                        builder.clear();
                        break;
                    }
                    if (ch == ',') {
                        tokens.push(builder.toString());
                        builder.clear();
                        tokens.emplace(",");
                        break;
                    }
                    builder.append(ch);
                }
                break;
        }
    }
    return true;
}

bool Json::parseObject(Tokens &tokens, ParseStack &stack) {
    bool has_end = false;
    auto object = stack.top();
    auto &object_ref = object->getDict();
    stack.pop();
    while (!tokens.empty()) {
        auto name = tokens.front();
        tokens.pop();
        if (name == "}") {
            has_end = true;
            break;
        }
        if (name == ",") {
            // token is ',' means there are more key-value pairs
            if (tokens.empty())
                return false;
            name = tokens.front();
            tokens.pop();
        }
        name = name.substr(1, name.size() - 2);

        if (tokens.empty())
            return false;
        // The token must be ":"
        if (tokens.front() != ":")
            return false;
        tokens.pop();

        if (tokens.empty())
            return false;
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // value is a ObjectData
            auto sub_object = object_ref.setRef(name, Value::dict()).get();
            stack.push(object);
            stack.push(sub_object);
            return true;
        }
        if (value == "[") {
            // value is an ArrayData
            auto sub_array = object_ref.setRef(name, Value::list()).get();
            stack.push(object);
            stack.push(sub_array);
            return true;
        }
        // value should be a BasicData, but it may be a keyword
        if (isKeyword(value.c_str()))
            return false;
        object_ref.set(name, parseBasic(value));
    }

    if (has_end) {
        return true;
    }
    return false;
}

bool Json::parseArray(Tokens &tokens, ParseStack &stack) {
    // the array exists keyword check, will not trigger end with check
    auto array = stack.top();
    auto &array_ref = array->getList();
    stack.pop();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if (token == "]") {
            // hasEnd = true;
            break;
        }
        if (token == ",") {
            // token is ',' means there are more values
            if (tokens.empty())
                return false;
            token = tokens.front();
            tokens.pop();
        }

        if (token == "{") {
            // value is a ObjectData
            auto sub_object = array_ref.appendRef(Value::dict()).get();
            stack.push(array);
            stack.push(sub_object);
            return true;
        }
        if (token == "[") {
            // value is an ArrayData
            auto sub_object = array_ref.appendRef(Value::list()).get();
            stack.push(array);
            stack.push(sub_object);
            return true;
        }
        // value should be a BasicData, but it may be a keyword
        if (isKeyword(token.c_str()))
            return false;
        array_ref.append(parseBasic(token));
    }

    return true;
}

Value Json::parse(Tokens &tokens) {
    Value root_value;
    if (tokens.front() == "{") {
        root_value = Value::dict();
    } else if (tokens.front() == "[") {
        root_value = Value::list();
    } else {
        return {};
    }

    tokens.pop();
    ParseStack stack;
    stack.push(&root_value);

    while (!stack.empty()) {
        auto current_value = stack.top();
        bool rt = false;
        if (current_value->isDict()) {
            rt = parseObject(tokens, stack);
        } else if (current_value->isList()) {
            rt = parseArray(tokens, stack);
        }
        if (!rt) {
            return {};
        }
    }

    return root_value;
}

Value Json::parseBasic(const std::string &value) {
    if (value == "null") {
        return {};
    }
    if (value == "true") {
        return Value(true);
    }
    if (value == "false") {
        return Value(false);
    }
    if (value.compare(0, 1, "\"") == 0) {
        return Value(value.substr(1, value.size() - 2));
    }
    auto is_float = value.find('.', 1);
    if (is_float != std::string::npos) {
        return Value(std::stod(value));
    }
    return Value(static_cast<Value::Integer>(std::stoll(value)));
}

Value Json::parse(io::InputStream *input) {
    Tokens tokens;
    if (!tokenizer(input, tokens)) {
        return {};
    }

    if (tokens.empty()) {
        return {};
    }

    return parse(tokens);
}

#define CONST_WRITE(stream, const_string)                                          \
    if (stream->write(const_string, strlen(const_string)) != strlen(const_string)) \
    return false

#define STRING_WRITE(stream, string)                                  \
    if (stream->write(string.data(), string.size()) != string.size()) \
    return false

bool Json::streamifyObject(
    io::OutputStream *out,
    StreamifyStack &stack,
    StreamifyIterStack &map_iter_stack
) {
    auto iter = map_iter_stack.top();
    map_iter_stack.pop();
    auto &[object, count] = stack.top();
    auto &object_ref = object->getDict();
    stack.pop();
    if (object_ref.empty()) {
        CONST_WRITE(out, "{}");
        return true;
    }
    if (count == object_ref.size()) {
        CONST_WRITE(out, "}");
        return true;
    }
    while (iter != object_ref.end()) {
        auto name = iter->first;
        auto value = iter->second;
        if (count == 0) {
            CONST_WRITE(out, "{");
        }
        if (count > 0 && count < object_ref.size()) {
            CONST_WRITE(out, ",");
        }
        ++iter;
        count += 1;
        CONST_WRITE(out, "\"");
        STRING_WRITE(out, name);
        CONST_WRITE(out, "\":");
        if (value->isDict()) {
            stack.emplace(object, count);
            stack.emplace(value.get(), 0);
            map_iter_stack.emplace(iter);
            map_iter_stack.emplace(value->getDict().begin());
            return true;
        }
        if (value->isList()) {
            stack.emplace(object, count);
            stack.emplace(value.get(), 0);
            map_iter_stack.emplace(iter);
            return true;
        }
        if (!streamifyBasic(out, value)) {
            return false;
        }
        if (count == object_ref.size()) {
            CONST_WRITE(out, "}");
            return true;
        }
    }
    return true;
}

bool Json::streamifyArray(
    io::OutputStream *out,
    StreamifyStack &stack,
    StreamifyIterStack &map_iter_stack
) {
    auto &[array, count] = stack.top();
    auto &array_ref = array->getList();
    stack.pop();
    if (array_ref.empty()) {
        CONST_WRITE(out, "[]");
        return true;
    }
    if (count == array_ref.size()) {
        CONST_WRITE(out, "]");
        return true;
    }
    while (count < array_ref.size()) {
        auto value = array_ref[count];
        if (count == 0) {
            CONST_WRITE(out, "[");
        }
        if (count > 0 && count < array_ref.size()) {
            CONST_WRITE(out, ",");
        }
        count += 1;
        if (value->isDict()) {
            stack.emplace(array, count);
            stack.emplace(value.get(), 0);
            map_iter_stack.emplace(value->getDict().begin());
            return true;
        }
        if (value->isList()) {
            stack.emplace(array, count);
            stack.emplace(value.get(), 0);
            return true;
        }
        if (!streamifyBasic(out, value)) {
            return false;
        }
        if (count == array_ref.size()) {
            CONST_WRITE(out, "]");
            return true;
        }
    }
    return true;
}

bool Json::streamifyBasic(io::OutputStream *out, const std::shared_ptr<Value> &value) {
    assert(value);
    if (value->isNull()) {
        CONST_WRITE(out, "null");
    } else if (value->isBool()) {
        if (value->getBool()) {
            CONST_WRITE(out, "true");
        } else {
            CONST_WRITE(out, "false");
        }
    } else if (value->isDouble() || value->isInt() || value->isString()) {
        auto v = value->toStringBasic();
        STRING_WRITE(out, v);
    } else {
        return false;
    }
    return true;
}

bool Json::streamify(io::OutputStream *out, Value &value) {
    StreamifyStack stack;
    StreamifyIterStack map_iter_stack;
    stack.emplace(&value, 0);
    if (value.isDict()) {
        map_iter_stack.emplace(value.getDict().begin());
    }

    while (!stack.empty()) {
        auto &[current_value, first] = stack.top();
        bool rt = false;
        if (current_value->isDict()) {
            rt = streamifyObject(out, stack, map_iter_stack);
        } else if (current_value->isList()) {
            rt = streamifyArray(out, stack, map_iter_stack);
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