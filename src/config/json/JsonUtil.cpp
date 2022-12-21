#include <sese/config/json/JsonUtil.h>
#include <sese/BufferedOutputStream.h>
#include <sese/BufferedInputStream.h>
#include <sese/text/StringBuilder.h>
#include <sese/Util.h>

using namespace sese::json;

ObjectData::Ptr JsonUtil::deserialize(const InputStream::Ptr &inputStream, size_t level) noexcept {
    // 此处懒得处理不合规格的格式，直接 catch
    try {
        Tokens tokens;
        if (!tokenizer(inputStream, tokens)) {
            return nullptr;
        }

        // 第一个 token 必是 '{'，直接弹出
        tokens.pop();
        return createObject(tokens, level);
    } catch (...) {
        return nullptr;
    }
}

void JsonUtil::serialize(const ObjectData::Ptr &object, const OutputStream::Ptr &outputStream) noexcept {
    auto bufferedStream = std::make_shared<BufferedOutputStream>(outputStream);
    serializeObject(object, bufferedStream);
    bufferedStream->flush();
}

bool JsonUtil::tokenizer(const InputStream::Ptr &inputStream, Tokens &tokens) noexcept {
    char ch;
    text::StringBuilder builder;

    int64_t len;
    while ((len = inputStream->read(&ch, 1 * sizeof(char))) != 0) {
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
                while ((len = inputStream->read(&ch, 1 * sizeof(char))) != 0) {
                    // 说明是转义字符，需要处理
                    if (ch == '\\') {
                        if (inputStream->read(&ch, 1 * sizeof(char)) == 0) {
                            // 字符串不完整
                            return false;
                        }
                        switch (ch) {
                            case '\"':
                            case '\\':
                            case '/':
                                builder.append(ch);
                                break;
                            case 'b':
                                builder.append('\b');
                                break;
                            case 'f':
                                builder.append('\f');
                                break;
                            case 'n':
                                builder.append('\n');
                                break;
                            case 'r':
                                builder.append('\r');
                                break;
                            case 't':
                                builder.append('\t');
                                break;
                            default:
                                break;
                        }
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
                while ((len = inputStream->read(&ch, 1 * sizeof(char))) != 0) {
                    if (sese::isSpace(ch)) {
                        tokens.push(builder.toString());
                        builder.clear();
                        break;
                    } else if (ch == ',') {
                        tokens.push(builder.toString());
                        builder.clear();
                        tokens.push(",");
                        break;
                    } else if (ch == '}' || ch == ']') {
                        tokens.push(builder.toString());
                        builder.clear();
                        tokens.push({ch});
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
    if (level == 0) { return nullptr; }
    auto object = std::make_shared<ObjectData>();
    while (!tokens.empty()) {
        auto name = tokens.front();
        tokens.pop();
        if (name == "}") return object;
        else if (name == ",") {
            // token 为 ',' 说明接下来还有键值对
            name = tokens.front();
            tokens.pop();
        }
        name = name.substr(1, name.size() - 2);

        // 此处 token 必是 ':'，故直接弹出
        tokens.pop();
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // 值是一个 ObjectData
            level--;
            ObjectData::Ptr subObject = createObject(tokens, level);
            if (subObject == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                object->set(name, subObject);
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
    return object;
}

ArrayData::Ptr JsonUtil::createArray(Tokens &tokens, size_t level) noexcept {
    if (level == 0) { return nullptr; }
    auto array = std::make_shared<ArrayData>();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if (token == "]") return array;
        else if (token == ",") {
            // token 为 ',' 说明接下来还有值
            token = tokens.front();
            tokens.pop();
        }

        if (token == "{") {
            // 值是一个 ObjectData
            level--;
            ObjectData::Ptr subObject = createObject(tokens, level);
            if (subObject == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(subObject);
            }
            level++;
        } else if (token == "[") {
            // 值是一个 ArrayData
            level--;
            ArrayData::Ptr subArray = createArray(tokens, level);
            if (array == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(subArray);
            }
            level++;
        } else {
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
    return array;
}

void JsonUtil::serializeObject(const ObjectData::Ptr &object, const OutputStream::Ptr &outputStream) noexcept {
    bool isFirst = true;
    outputStream->write("{", 1);
    for (auto iterator = object->begin(); iterator != object->end(); iterator++) {
        if (isFirst) {
            isFirst = false;
        } else {
            outputStream->write(",", 1);
        }
        auto data = iterator->second;
        auto name = iterator->first;
        outputStream->write("\"", 1);
        outputStream->write(name.c_str(), name.length());
        outputStream->write("\":", 2);

        if (data->getType() == DataType::Object) {
            serializeObject(std::dynamic_pointer_cast<ObjectData>(data), outputStream);
        } else if (data->getType() == DataType::Array) {
            serializeArray(std::dynamic_pointer_cast<ArrayData>(data), outputStream);
        } else {
            auto raw = std::dynamic_pointer_cast<BasicData>(data)->raw();
            if (raw[0] == '\"') {
                auto len = raw.length() - 2;
                const char *p = raw.c_str() + 1;
                outputStream->write("\"", 1);
                for (int i = 0; i < len; ++i) {
                    switch (p[i]) {
                        case '\"':
                        case '\'':
                        case '/':
                            outputStream->write("\\", 1);
                            outputStream->write(&p[i], 1);
                            break;
                        case '\b':
                            outputStream->write("\\b", 2);
                            break;
                        case '\f':
                            outputStream->write("\\f", 2);
                            break;
                        case '\n':
                            outputStream->write("\\n", 2);
                            break;
                        case '\r':
                            outputStream->write("\\r", 2);
                            break;
                        case '\t':
                            outputStream->write("\\t", 2);
                            break;
                        default:
                            outputStream->write(&p[i], 1);
                            break;
                    }
                }
                outputStream->write("\"", 1);
            } else {
                outputStream->write(raw.c_str(), raw.length());
            }
        }
    }
    outputStream->write("}", 1);
}

void JsonUtil::serializeArray(const ArrayData::Ptr &array, const OutputStream::Ptr &outputStream) noexcept {
    bool isFirst = true;
    outputStream->write("[", 1);
    for (auto iterator = array->begin(); iterator != array->end(); iterator++) {
        if (isFirst) {
            isFirst = false;
        } else {
            outputStream->write(",", 1);
        }

        auto data = *iterator;
        if (data->getType() == DataType::Object) {
            serializeObject(std::dynamic_pointer_cast<ObjectData>(data), outputStream);
        } else if (data->getType() == DataType::Array) {
            serializeArray(std::dynamic_pointer_cast<ArrayData>(data), outputStream);
        } else {
            auto raw = std::dynamic_pointer_cast<BasicData>(data)->raw();
            outputStream->write(raw.c_str(), raw.length());
        }
    }
    outputStream->write("]", 1);
}