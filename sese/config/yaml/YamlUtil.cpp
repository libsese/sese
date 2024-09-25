#include "sese/config/yaml/YamlUtil.h"
#include "sese/text/StringBuilder.h"

#include <array>

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

sese::yaml::Data::Ptr sese::yaml::YamlUtil::deserialize(InputStream *input, size_t level) noexcept {
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
    if (top_tokens.empty())
        return nullptr; // GCOVR_EXCL_LINE
    if (top_tokens[0] == "-") {
        // 根元素是数组
        return createArray(tokens_queue, level);
    } else {
        // 根元素是对象
        return createObject(tokens_queue, level);
    }
}

void sese::yaml::YamlUtil::serialize(const Data::Ptr &data, OutputStream *output) noexcept {
    if (data->getType() == DataType::OBJECT_DATA) {
        auto sub = dynamic_cast<ObjectData *>(data.get()); // GCOVR_EXCL_LINE
        serializeObject(sub, output, 0);
    } else if (data->getType() == DataType::ARRAY_DATA) {
        auto sub = dynamic_cast<ArrayData *>(data.get()); // GCOVR_EXCL_LINE
        serializeArray(sub, output, 0);
    } else {
        // BasicData
        return;
    }
}

int sese::yaml::YamlUtil::getSpaceCount(const std::string &line) noexcept {
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

std::tuple<int, std::string> sese::yaml::YamlUtil::getLine(InputStream *input) noexcept {
    sese::text::StringBuilder builder(1024);
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
    auto count = YamlUtil::getSpaceCount(str);
    return {count, str.substr(count)};
}

std::vector<std::string> sese::yaml::YamlUtil::tokenizer(const std::string &line) noexcept {
    std::vector<std::string> vector;
    sese::text::StringBuilder builder(1024);
    bool is_str = false; // 是否是字符串
    bool quot1 = false;  // 单引号 - 不转义
    bool quot2 = false;  // 双引号 - 转义
    bool tran = false;   // 是否转义
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
                // 不再自动处理转义字符
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

sese::yaml::ObjectData::Ptr
sese::yaml::YamlUtil::createObject(
        sese::yaml::YamlUtil::TokensQueue &tokens_queue,
        size_t level
) noexcept {
    if (level == 0)
        return nullptr;

    auto result = std::make_shared<ObjectData>();
    // 对象无子元素
    if (tokens_queue.empty())
        return result; // GCOVR_EXCL_LINE
    int count = std::get<0>(tokens_queue.front());

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        auto current_count = std::get<0>(count_and_tokens);
        auto current_tokens = std::get<1>(count_and_tokens);

        if (count == current_count) {
            if (current_tokens.size() == 3) {
                if (current_tokens[1] != ":") {
                    return nullptr;
                }
                // 普通键值对
                std::string key = current_tokens[0];
                std::string value = current_tokens[2];
                auto value_object = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    value_object->setNull(true);
                } else {
                    value_object->setDataAs<std::string>(value);
                }
                result->set(key, value_object);
                tokens_queue.pop();
            } else if (current_tokens.size() == 2) {
                // 可能是空对象，也可能是新对象或新数组
                auto current_count_and_tokens = tokens_queue.front();
                tokens_queue.pop();
                if (tokens_queue.empty()) {
                    // 不存在下一行且当前元素为 null
                    auto value_object = std::make_shared<BasicData>();
                    value_object->setNull(true);
                    result->set(current_tokens[0], value_object);
                } else {
                    // 存在下一行
                    auto next_count_and_tokens = tokens_queue.front();
                    auto next_count = std::get<0>(next_count_and_tokens);
                    auto next_tokens = std::get<1>(next_count_and_tokens);
                    if (next_count == count) {
                        // 当前行元素是 null
                        // 下一个元素依然是本对象所属
                        auto value_object = std::make_shared<BasicData>();
                        value_object->setNull(true);
                        result->set(current_tokens[0], value_object);
                    } else if (next_count < count) {
                        // 当前行元素是 null
                        // 下一个元素不是本对象所属
                        const auto VALUE_OBJECT = std::make_shared<BasicData>();
                        VALUE_OBJECT->setNull(true);
                        result->set(current_tokens[0], VALUE_OBJECT);
                        return result;
                    } else if (next_count > count) {
                        // 新的数组
                        if (next_tokens[0] == "-") {
                            if (auto value_object = createArray(tokens_queue, level - 1)) {
                                result->set(current_tokens[0], value_object);
                            } else {
                                return nullptr;
                            }
                        }
                        // 新的对象
                        else {
                            if (auto value_object = createObject(tokens_queue, level - 1)) {
                                result->set(current_tokens[0], value_object);
                            } else {
                                return nullptr;
                            }
                        }
                    }
                }
            }
        } else if (count > current_count) {
            // 当前对象结束
            return result;
        }
        // 错误 - 这几乎不可能发生
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return result;
}

sese::yaml::ArrayData::Ptr
sese::yaml::YamlUtil::createArray(
        sese::yaml::YamlUtil::TokensQueue &tokens_queue,
        size_t level
) noexcept {
    if (level == 0)
        return nullptr;

    auto result = std::make_shared<ArrayData>();
    // 对象无子元素
    if (tokens_queue.empty())
        return result;
    const int COUNT = std::get<0>(tokens_queue.front());

    while (!tokens_queue.empty()) {
        decltype(auto) count_and_tokens = tokens_queue.front();
        const auto CURRENT_COUNT = std::get<0>(count_and_tokens);
        auto current_tokens = std::get<1>(count_and_tokens);

        if (COUNT == CURRENT_COUNT) {
            if (current_tokens.size() == 1) {
                auto value_object = std::make_shared<BasicData>();
                value_object->setNull(true);
                result->push(value_object);
                tokens_queue.pop();
            }
            else if (current_tokens.size() == 2) {
                std::string value = current_tokens[1];
                auto value_object = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    value_object->setNull(true);
                } else {
                    value_object->setDataAs<std::string>(value);
                }
                result->push(value_object);
                tokens_queue.pop();
            }
            else if (current_tokens.size() == 3) {
                // 新的对象或数组
                auto current_count_and_tokens = tokens_queue.front();
                tokens_queue.pop();
                if (tokens_queue.empty()) {
                    // 不存在下一行，跳过该对象
                    return result;
                } else {
                    auto next_count_and_tokens = tokens_queue.front();
                    const auto NEXT_COUNT = std::get<0>(next_count_and_tokens);
                    auto next_tokens = std::get<1>(next_count_and_tokens);
                    if (NEXT_COUNT > COUNT) {
                        if (next_tokens[0] == "-") {
                            if (auto value_object = createArray(tokens_queue, level - 1)) {
                                result->push(value_object);
                            } else {
                                return nullptr;
                            }
                        } else {
                            auto value_object = createObject(tokens_queue, level - 1);
                            if (value_object) {
                                result->push(value_object);
                            } else {
                                return nullptr;
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
                auto value_object = std::make_shared<BasicData>();
                value_object->setDataAs<std::string>(current_tokens[3]);
                auto key_object = std::make_shared<ObjectData>();
                key_object->set(current_tokens[1], value_object);
                result->push(key_object);
                tokens_queue.pop();
            }
            else {
                return nullptr;
            }
        } else if (COUNT > CURRENT_COUNT) {
            // 当前数组结束
            return result;
        }
        // 错误 - 这几乎不会发生
        // GCOVR_EXCL_START
        else {
            tokens_queue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return result;
}

constexpr auto GET_SPACE_ARRAY = []() {
    std::array<char, 1024> array{};
    for (decltype(auto) item: array) {
        item = ' ';
    }
    return array;
};

void sese::yaml::YamlUtil::writeSpace(size_t count, OutputStream *output) noexcept {
    const auto BUFFER = GET_SPACE_ARRAY();
    const auto SIZE = std::min<size_t>(1024, count);
    output->write(BUFFER.data(), SIZE);
}

void sese::yaml::YamlUtil::serializeObject(ObjectData *object_data, OutputStream *output, size_t level) noexcept {
    for (auto &[fst, snd]: *object_data) {
        if (snd->getType() == DataType::OBJECT_DATA) {
            if (const auto SUB = dynamic_cast<ObjectData *>(snd.get()); !SUB->empty()) {
                writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                serializeObject(SUB, output, level + 1);
            }
        } else if (snd->getType() == DataType::ARRAY_DATA) {
            if (const auto SUB = dynamic_cast<ArrayData *>(snd.get()); !SUB->empty()) {
                writeSpace(level * 2, output);
                output->write(fst.c_str(), fst.length());
                output->write(":\n", 2);
                serializeArray(SUB, output, level + 1);
            }
        } else {
            const auto SUB = dynamic_cast<BasicData *>(snd.get()); // GCOVR_EXCL_LINE
            auto data = SUB->getDataAs<std::string>("");
            writeSpace(level * 2, output);
            output->write(fst.c_str(), fst.length());
            output->write(": ", 2);
            if (SUB->isNull()) {
                output->write("~", 1);
            } else {
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}

void sese::yaml::YamlUtil::serializeArray(ArrayData *array_data, OutputStream *output, size_t level) noexcept {
    auto count = 0;
    for (decltype(auto) item: *array_data) {
        if (item->getType() == DataType::OBJECT_DATA) {
            if (const auto SUB = dynamic_cast<ObjectData *>(item.get()); !SUB->empty()) { // GCOVR_EXCL_LINE
                auto name = "element_" + std::to_string(count);
                count += 1;
                writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                serializeObject(SUB, output, level + 1);
            }
        } else if (item->getType() == DataType::ARRAY_DATA) {
            if (const auto SUB = dynamic_cast<ArrayData *>(item.get()); !SUB->empty()) { // GCOVR_EXCL_LINE
                auto name = "element_" + std::to_string(count);
                count += 1;
                writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                serializeArray(SUB, output, level + 1);
            }
        } else {
            const auto SUB = dynamic_cast<BasicData *>(item.get()); // GCOVR_EXCL_LINE
            auto data = SUB->getDataAs<std::string>("");
            writeSpace(level * 2, output);
            output->write("- ", 2);
            if (SUB->isNull()) {
                output->write("~", 1);
            } else {
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}