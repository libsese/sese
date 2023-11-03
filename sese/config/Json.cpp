#include <sese/config/Json.h>
#include <sese/config/json/JsonUtil.h>

#include <cassert>

using sese::Json;
using sese::Value;
using sese::json::JsonUtil;

/// 判断一个 token 是否为 json 关键字
/// \param str token
/// \return 结果
inline bool isKeyword(const char *str) {
    if (str[0] == '}' || str[0] == ']' || str[0] == ':' || str[0] == ',') {
        return true;
    } else {
        return false;
    }
}

Value Json::parseObject(Json::Tokens &tokens, size_t level) {
    bool hasEnd = false;
    if (level == 0) { return {}; }
    auto object = Value::Dict();
    while (!tokens.empty()) {
        auto name = tokens.front();
        tokens.pop();
        if (name == "}") {
            hasEnd = true;
            break;
        } else if (name == ",") {
            // token 为 ',' 说明接下来还有键值对
            if (tokens.empty()) return {}; // GCOVR_EXCL_LINE
            name = tokens.front();
            tokens.pop();
        }
        name = name.substr(1, name.size() - 2);

        if (tokens.empty()) return {}; // GCOVR_EXCL_LINE
        // 此处 token 必是 ":"
        if (tokens.front() != ":") return {};
        tokens.pop();

        if (tokens.empty()) return {}; // GCOVR_EXCL_LINE
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // 值是一个 ObjectData
            level--;
            auto subObject = parseObject(tokens, level);
            if (subObject.isNull()) {
                // 解析错误，直接返回
                return {};
            } else {
                object.set(name, std::move(subObject));
            }
            level++;
        } else if (value == "[") {
            // 值是一个 ArrayData
            level--;
            auto array = parseArray(tokens, level);
            if (array.isNull()) {
                // 解析错误，直接返回
                return {};
            } else {
                object.set(name, std::move(array));
            }
            level++;
        } else {
            // 本应存在值的地方实际上是关键字
            if (isKeyword(value.c_str())) return {};
            // 值是一个 BasicData
            object.set(name, parseBasic(value));
        }
    }

    if (hasEnd) {
        return Value(std::move(object));
    } else {
        return {};
    }
}

Value Json::parseArray(sese::Json::Tokens &tokens, size_t level) {
    // 数组存在关键字检查，不会触发 end with 检查
    // bool hasEnd = false;
    if (level == 0) { return {}; }
    auto array = Value::List();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if (token == "]") {
            // hasEnd = true;
            break;
        } else if (token == ",") {
            // token 为 ',' 说明接下来还有值
            if (tokens.empty()) return {}; // GCOVR_EXCL_LINE
            token = tokens.front();
            tokens.pop();
        }

        if (token == "{") {
            // 值是一个 ObjectData
            level--;
            auto subObject = parseObject(tokens, level);
            if (subObject.isNull()) {
                // 解析错误，直接返回
                return {};
            } else {
                array.append(std::move(subObject));
            }
            level++;
        } else if (token == "[") {
            // 值是一个 ArrayData
            level--;
            auto subArray = parseArray(tokens, level);
            if (subArray.isNull()) {
                // 解析错误，直接返回
                return {};
            } else {
                array.append(std::move(subArray));
            }
            level++;
        } else {
            // 本应存在值的地方实际上是关键字
            if (isKeyword(token.c_str())) return {};
            // 值是一个 BasicData
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
        auto isFloat = value.find('.', 1);
        if (isFloat != std::string::npos) {
            return Value(std::stod(value));
        } else {
            return Value(std::stoi(value));
        }
    }
}

void Json::streamifyObject(io::OutputStream *out, const Value::Dict &object) {
    bool isFirst = true;
    out->write("{", 1);
    for (const auto &iterator: object) {
        if (isFirst) {
            isFirst = false;
        } else {
            out->write(",", 1);
        }
        auto data = iterator.second;
        auto name = iterator.first;
        out->write("\"", 1);
        out->write(name.c_str(), name.length());
        out->write("\":", 2);

        if (data->getType() == Value::Type::Dict) {
            streamifyObject(out, data->getDict()); // GCOVR_EXCL_LINE
        } else if (data->getType() == Value::Type::List) {
            streamifyArray(out, data->getList()); // GCOVR_EXCL_LINE
        } else {
            streamifyBasic(out, data); // GCOVR_EXCL_LINE
        }
    }
    out->write("}", 1);
}

void Json::streamifyArray(io::OutputStream *out, const Value::List &array) {
    bool isFirst = true;
    out->write("[", 1);
    for (const auto &data: array) {
        if (isFirst) {
            isFirst = false;
        } else {
            out->write(",", 1);
        }

        if (data.getType() == Value::Type::Dict) {
            streamifyObject(out, data.getDict()); // GCOVR_EXCL_LINE
        } else if (data.getType() == Value::Type::List) {
            streamifyArray(out, data.getList()); // GCOVR_EXCL_LINE
        } else {
            streamifyBasic(out, &data); // GCOVR_EXCL_LINE
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
    if (!JsonUtil::tokenizer(input, tokens)) {
        return {};
    }

    if (tokens.empty()) {
        return {};
    }

    // 第一个 token 必是 '{'，直接弹出
    if (tokens.front() != "{") return {};
    tokens.pop();
    return parseObject(tokens, level);
}

void Json::streamify(io::OutputStream *out, const Value::Dict &dict) {
    streamifyObject(out, dict);
}