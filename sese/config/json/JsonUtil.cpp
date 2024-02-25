#include <sese/config/json/JsonUtil.h>
#include "sese/io/BufferedOutputStream.h"
#include "sese/io/BufferedInputStream.h"
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

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

using namespace sese::json;

ObjectData::Ptr JsonUtil::deserialize(const InputStream::Ptr &input_stream, size_t level) noexcept {
    return deserialize(input_stream.get(), level);
}

void JsonUtil::serialize(const ObjectData::Ptr &object, const OutputStream::Ptr &output_stream) noexcept {
    serializeObject(object.get(), output_stream.get());
}

ObjectData::Ptr JsonUtil::deserialize(InputStream *input_stream, size_t level) noexcept {
    Tokens tokens;
    if (!tokenizer(input_stream, tokens)) {
        return nullptr;
    }

    if (tokens.empty()) {
        return nullptr;
    }

    // 第一个 token 必是 '{'，直接弹出
    if (tokens.front() != "{") return nullptr;
    tokens.pop();
    return createObject(tokens, level);
}

void JsonUtil::serialize(ObjectData *object, OutputStream *output_stream) noexcept {
    serializeObject(object, output_stream);
}

bool JsonUtil::tokenizer(InputStream *input_stream, Tokens &tokens) noexcept {
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
                // 一般的 token
                tokens.push({ch});
                break;
            case '\"':
                builder.append("\"");
                // 字符串 token，需要处理转义符号
                while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
                    // 说明是转义字符
                    if (ch == '\\') {
                        if (input_stream->read(&ch, 1 * sizeof(char)) == 0) {
                            // 字符串不完整
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
                // 普通的值 token
                if (sese::isSpace(ch)) break;
                builder.append(ch);
                while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
                    //fix: 此处多加关键字判断
                    if (isKeyword(&ch)) {
                        tokens.push(builder.toString());
                        builder.clear();
                        tokens.push({ch});
                        break;
                    } else if (sese::isSpace(ch)) {
                        tokens.push(builder.toString());
                        builder.clear();
                        break;
                    } else if (ch == ',') {
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

ObjectData::Ptr JsonUtil::createObject(Tokens &tokens, size_t level) noexcept {
    bool has_end = false;
    if (level == 0) { return nullptr; }
    auto object = std::make_shared<ObjectData>();
    while (!tokens.empty()) {
        auto name = tokens.front();
        tokens.pop();
        if (name == "}") {
            has_end = true;
            break;
        } else if (name == ",") {
            // token 为 ',' 说明接下来还有键值对
            if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
            name = tokens.front();
            tokens.pop();
        }
        name = name.substr(1, name.size() - 2);

        if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
        // 此处 token 必是 ":"
        if (tokens.front() != ":") return nullptr;
        tokens.pop();

        if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // 值是一个 ObjectData
            level--;
            if (auto sub_object = createObject(tokens, level);sub_object == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                object->set(name, sub_object);
            }
            level++;
        } else if (value == "[") {
            // 值是一个 ArrayData
            level--;
            ArrayData::Ptr array = createArray(tokens, level);
            if (array == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                object->set(name, array);
            }
            level++;
        } else {
            // 本应存在值的地方实际上是关键字
            if (isKeyword(value.c_str())) return nullptr;
            // 值是一个 BasicData
            BasicData::Ptr data;
            if (value == "null") {
                data = std::make_shared<BasicData>();
            } else {
                data = std::make_shared<BasicData>(value);
            }
            object->set(name, data);
        }
    }

    if (has_end) {
        return object;
    } else {
        return nullptr;
    }
}

ArrayData::Ptr JsonUtil::createArray(Tokens &tokens, size_t level) noexcept {
    // 数组存在关键字检查，不会触发 end with 检查
    // bool hasEnd = false;
    if (level == 0) { return nullptr; }
    auto array = std::make_shared<ArrayData>();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if (token == "]") {
            // hasEnd = true;
            break;
        } else if (token == ",") {
            // token 为 ',' 说明接下来还有值
            if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
            token = tokens.front();
            tokens.pop();
        }

        if (token == "{") {
            // 值是一个 ObjectData
            level--;
            ObjectData::Ptr sub_object = createObject(tokens, level);
            if (sub_object == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(sub_object);
            }
            level++;
        } else if (token == "[") {
            // 值是一个 ArrayData
            level--;
            ArrayData::Ptr sub_array = createArray(tokens, level);
            if (sub_array == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(sub_array);
            }
            level++;
        } else {
            // 本应存在值的地方实际上是关键字
            if (isKeyword(token.c_str())) return nullptr;
            // 值是一个 BasicData
            BasicData::Ptr data;
            if (token == "null") {
                data = std::make_shared<BasicData>();
            } else {
                data = std::make_shared<BasicData>(token);
            }
            array->push(data);
        }
    }

    // if (hasEnd) {
    return array;
    // } else {
    //    return nullptr;
    // }
}

void JsonUtil::serializeObject(ObjectData *object, OutputStream *output_stream) noexcept {
    bool is_first = true;
    output_stream->write("{", 1);
    for (auto iterator = object->begin(); iterator != object->end(); iterator++) {
        if (is_first) {
            is_first = false;
        } else {
            output_stream->write(",", 1);
        }
        auto data = iterator->second;
        auto name = iterator->first;
        output_stream->write("\"", 1);
        output_stream->write(name.c_str(), name.length());
        output_stream->write("\":", 2);

        if (data->getType() == DataType::OBJECT) {
            serializeObject(dynamic_cast<ObjectData *>(data.get()), output_stream); // GCOVR_EXCL_LINE
        } else if (data->getType() == DataType::ARRAY) {
            serializeArray(dynamic_cast<ArrayData *>(data.get()), output_stream); // GCOVR_EXCL_LINE
        } else {
            auto raw = std::dynamic_pointer_cast<BasicData>(data)->raw();
            // if (raw[0] == '\"') {
            //     auto len = raw.length() - 2;
            //     const char *p = raw.c_str() + 1;
            //     outputStream->write("\"", 1);
            //     for (int i = 0; i < len; ++i) {
            //         switch (p[i]) {
            //             case '\"':
            //             case '\'':
            //             case '/':
            //                 outputStream->write("\\", 1);
            //                 outputStream->write(&p[i], 1);
            //                 break;
            //             case '\b':
            //                 outputStream->write("\\b", 2);
            //                 break;
            //             case '\f':
            //                 outputStream->write("\\f", 2);
            //                 break;
            //             case '\n':
            //                 outputStream->write("\\n", 2);
            //                 break;
            //             case '\r':
            //                 outputStream->write("\\r", 2);
            //                 break;
            //             case '\t':
            //                 outputStream->write("\\t", 2);
            //                 break;
            //             default:
            //                 outputStream->write(&p[i], 1);
            //                 break;
            //         }
            //     }
            //     outputStream->write("\"", 1);
            // } else {
            output_stream->write(raw.c_str(), raw.length());
            //}
        }
    }
    output_stream->write("}", 1);
}

void JsonUtil::serializeArray(ArrayData *array, OutputStream *output_stream) noexcept {
    bool is_first = true;
    output_stream->write("[", 1);
    for (auto iterator = array->begin(); iterator != array->end(); iterator++) {
        if (is_first) {
            is_first = false;
        } else {
            output_stream->write(",", 1);
        }

        auto data = *iterator;
        if (data->getType() == DataType::OBJECT) {
            serializeObject(dynamic_cast<ObjectData *>(data.get()), output_stream); // GCOVR_EXCL_LINE
        } else if (data->getType() == DataType::ARRAY) {
            serializeArray(dynamic_cast<ArrayData *>(data.get()), output_stream); // GCOVR_EXCL_LINE
        } else {
            auto raw = std::dynamic_pointer_cast<BasicData>(data)->raw();
            output_stream->write(raw.c_str(), raw.length());
        }
    }
    output_stream->write("]", 1);
}