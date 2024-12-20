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
    } else {
        return false;
    }
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

Value Json::parseObject(Json::Tokens &tokens, size_t level) {
    bool has_end = false;
    if (level == 0) {
        return {};
    }
    auto object = Value::Dict();
    while (!tokens.empty()) {
        auto name = tokens.front();
        tokens.pop();
        if (name == "}") {
            has_end = true;
            break;
        } else if (name == ",") {
            // token is ',' means there are more key-value pairs
            if (tokens.empty())
                return {};
            name = tokens.front();
            tokens.pop();
        }
        name = name.substr(1, name.size() - 2);

        if (tokens.empty())
            return {};
        // The token must be ":"
        if (tokens.front() != ":")
            return {};
        tokens.pop();

        if (tokens.empty())
            return {};
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // value is a ObjectData
            level--;
            auto sub_object = parseObject(tokens, level);
            if (sub_object.isNull()) {
                // parse error, just return
                return {};
            } else {
                object.set(name, std::move(sub_object));
            }
            level++;
        } else if (value == "[") {
            // value is an ArrayData
            level--;
            auto array = parseArray(tokens, level);
            if (array.isNull()) {
                // parse error, just return
                return {};
            } else {
                object.set(name, std::move(array));
            }
            level++;
        } else {
            // value should be a BasicData, but it may be a keyword
            if (isKeyword(value.c_str()))
                return {};
            object.set(name, parseBasic(value));
        }
    }

    if (has_end) {
        return Value(std::move(object));
    }
    return {};
}

Value Json::parseArray(sese::Json::Tokens &tokens, size_t level) {
    // the array exists keyword check, will not trigger end with check
    // bool hasEnd = false;
    if (level == 0) {
        return {};
    }
    auto array = Value::List();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if (token == "]") {
            // hasEnd = true;
            break;
        } else if (token == ",") {
            // token is ',' means there are more values
            if (tokens.empty())
                return {};
            token = tokens.front();
            tokens.pop();
        }

        if (token == "{") {
            // value is a ObjectData
            level--;
            auto sub_object = parseObject(tokens, level);
            if (sub_object.isNull()) {
                // parse error, just return
                return {};
            } else {
                array.append(std::move(sub_object));
            }
            level++;
        } else if (token == "[") {
            // value is an ArrayData
            level--;
            auto sub_array = parseArray(tokens, level);
            if (sub_array.isNull()) {
                // parse error, just return
                return {};
            } else {
                array.append(std::move(sub_array));
            }
            level++;
        } else {
            // value should be a BasicData, but it may be a keyword
            if (isKeyword(token.c_str()))
                return {};
            array.append(parseBasic(token));
        }
    }

    // if (hasEnd) {
    return Value(std::move(array));
    // } else {
    //    return nullptr;
    // }
}

Value Json::parseBasic(const std::string &value) {
    if (value == "null") {
        return {};
    } else if (value == "true") {
        // data = std::make_shared<BasicData>(value);
        return Value(true);
    } else if (value == "false") {
        return Value(false);
    } else if (value.compare(0, 1, "\"") == 0) {
        return Value(value.substr(1, value.size() - 2));
    } else {
        auto is_float = value.find('.', 1);
        if (is_float != std::string::npos) {
            return Value(std::stod(value));
        } else {
            return Value(static_cast<Value::Integer>(std::stoll(value)));
        }
    }
}

void Json::streamifyObject(io::OutputStream *out, const Value::Dict &object) {
    bool is_first = true;
    out->write("{", 1);
    for (const auto &iterator: object) {
        if (is_first) {
            is_first = false;
        } else {
            out->write(",", 1);
        }
        auto data = iterator.second;
        auto name = iterator.first;
        out->write("\"", 1);
        out->write(name.c_str(), name.length());
        out->write("\":", 2);

        if (data->getType() == Value::Type::DICT) {
            streamifyObject(out, data->getDict());
        } else if (data->getType() == Value::Type::LIST) {
            streamifyArray(out, data->getList());
        } else {
            streamifyBasic(out, data);
        }
    }
    out->write("}", 1);
}

void Json::streamifyArray(io::OutputStream *out, const Value::List &array) {
    bool is_first = true;
    out->write("[", 1);
    for (const auto &data: array) {
        if (is_first) {
            is_first = false;
        } else {
            out->write(",", 1);
        }

        if (data->getType() == Value::Type::DICT) {
            streamifyObject(out, data->getDict());
        } else if (data->getType() == Value::Type::LIST) {
            streamifyArray(out, data->getList());
        } else {
            streamifyBasic(out, data);
        }
    }
    out->write("]", 1);
}

void Json::streamifyBasic(io::OutputStream *out, const sese::Value *value) {
    assert(value);
    if (value->isNull()) {
        out->write("null", 4);
    } else if (value->isBool()) {
        out->write(value->getBool() ? "true" : "false", 4);
    } else if (value->isDouble() || value->isInt() || value->isString()) {
        auto v = value->toString();
        out->write(v.c_str(), v.size());
    }
}

Value Json::parse(io::InputStream *input, size_t level) {
    Tokens tokens;
    if (!tokenizer(input, tokens)) {
        return {};
    }

    if (tokens.empty()) {
        return {};
    }

    // The first token must be '{', pop it
    if (tokens.front() != "{")
        return {};
    tokens.pop();
    return parseObject(tokens, level);
}

void Json::streamify(io::OutputStream *out, const Value::Dict &dict) {
    streamifyObject(out, dict);
}

// GCOVR_EXCL_STOP