#include <sese/config/json/JsonUtil.h>
#include <sese/StringBuilder.h>
#include <sese/Util.h>

using namespace sese::json;

bool JsonUtil::tokenizer(const Stream::Ptr &inputStream, Tokens &tokens) noexcept {
    char ch;
    StringBuilder builder;

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
                    }

                    builder.append(ch);
                }
                break;
        }
    }
    return true;
}

ObjectData::Ptr JsonUtil::parser(Tokens &tokens) noexcept {
    // 第一个 token 必是 '{'，直接弹出
    tokens.pop();
    return createObject(tokens);
}

ObjectData::Ptr JsonUtil::createObject(Tokens &tokens) noexcept {
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

        // 此处 token 必是 ':'，故直接弹出
        tokens.pop();
        auto value = tokens.front();
        tokens.pop();

        if (value == "{") {
            // 值是一个 ObjectData
            ObjectData::Ptr subObject = createObject(tokens);
            if (subObject == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                object->set(name, subObject);
            }
        } else if (value == "[") {
            // 值是一个 ArrayData
            ArrayData::Ptr array = createArray(tokens);
            if (array == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                object->set(name, array);
            }
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

ArrayData::Ptr JsonUtil::createArray(Tokens &tokens) noexcept {
    auto array = std::make_shared<ArrayData>();
    while (!tokens.empty()) {
        auto token = tokens.front();
        tokens.pop();
        if(token == "]") return array;
        else if (token == ",") {
            // token 为 ',' 说明接下来还有值
            token = tokens.front();
            tokens.pop();
        }

        if(token == "{") {
            // 值是一个 ObjectData
            ObjectData::Ptr subObject = createObject(tokens);
            if (subObject == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(subObject);
            }
        }else if(token == "[") {
            // 值是一个 ArrayData
            ArrayData::Ptr subArray = createArray(tokens);
            if (array == nullptr) {
                // 解析错误，直接返回
                return nullptr;
            } else {
                array->push(subArray);
            }
        }else {
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
}