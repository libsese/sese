#include "sese/config/yaml/YamlUtil.h"
#include "sese/text/StringBuilder.h"

#include <array>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

sese::yaml::Data::Ptr sese::yaml::YamlUtil::deserialize(sese::InputStream *input, size_t level) noexcept {
    TokensQueue tokensQueue;
    while (true) {
        decltype(auto) line = sese::yaml::YamlUtil::getLine(input);
        if (std::get<1>(line).length() == 0) {
            break;
        }
        auto tokens = sese::yaml::YamlUtil::tokenizer(std::get<1>(line));
        tokensQueue.emplace(std::get<0>(line), tokens);
    }

    decltype(auto) top = tokensQueue.front();
    decltype(auto) topTokens = std::get<1>(top);
    if (topTokens.empty()) return nullptr;// GCOVR_EXCL_LINE
    if (topTokens[0] == "-") {
        // 根元素是数组
        return createArray(tokensQueue, level);
    } else {
        // 根元素是对象
        return createObject(tokensQueue, level);
    }
}

void sese::yaml::YamlUtil::serialize(const Data::Ptr &data, sese::OutputStream *output) noexcept {
    if (data->getType() == DataType::ObjectData) {
        auto sub = dynamic_cast<ObjectData *>(data.get());// GCOVR_EXCL_LINE
        serializeObject(sub, output, 0);
    } else if (data->getType() == DataType::ArrayData) {
        auto sub = dynamic_cast<ArrayData *>(data.get());// GCOVR_EXCL_LINE
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

std::tuple<int, std::string> sese::yaml::YamlUtil::getLine(sese::InputStream *input) noexcept {
    sese::text::StringBuilder builder(1024);
    char ch;
    while (true) {
        auto l = input->read(&ch, 1);
        if (l <= 0) {
            goto ret;
        }
        if (ch == '\r' || ch == '\n') {
            if (builder.length() > 0) {
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
    bool isStr = false;// 是否是字符串
    bool quot1 = false;// 单引号 - 不转义
    bool quot2 = false;// 双引号 - 转义
    bool tran = false; // 是否转义
    for (decltype(auto) ch: line) {
        if (quot1) {
            if (ch == '\'') {
                isStr = false;
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
                isStr = false;
                quot2 = false;
                vector.emplace_back(builder.toString());
                builder.clear();
            } else if (ch == '\\') {
                tran = true;
            } else {
                builder << ch;
            }
        } else if (isStr) {
            if (ch == ':') {
                isStr = false;
                vector.emplace_back(builder.toString());
                vector.emplace_back(":");
                builder.clear();
            } else {
                builder << ch;
            }
        } else if (ch == '\'') {
            quot1 = true;
            isStr = true;
        } else if (ch == '\"') {
            quot2 = true;
            isStr = true;
        } else if (ch == ':') {
            vector.emplace_back(":");
        } else if (ch == '-') {
            vector.emplace_back("-");
        } else if (ch == ' ') {
            continue;
        } else {
            builder << ch;
            isStr = true;
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
        sese::yaml::YamlUtil::TokensQueue &tokensQueue,
        size_t level
) noexcept {
    if (level == 0) return nullptr;

    auto result = std::make_shared<ObjectData>();
    // 对象无子元素
    if (tokensQueue.empty()) return result;// GCOVR_EXCL_LINE
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 3) {
                if (currentTokens[1] != ":")  {
                    return nullptr;
                }
                // 普通键值对
                std::string key = currentTokens[0];
                std::string value = currentTokens[2];
                auto valueObject = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    valueObject->setNull(true);
                } else {
                    valueObject->setDataAs<std::string>(value);
                }
                result->set(key, valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                // 可能是空对象，也可能是新对象或新数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行且当前元素为 null
                    auto valueObject = std::make_shared<BasicData>();
                    valueObject->setNull(true);
                    result->set(currentTokens[0], valueObject);
                } else {
                    // 存在下一行
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount == count) {
                        // 当前行元素是 null
                        // 下一个元素依然是本对象所属
                        auto valueObject = std::make_shared<BasicData>();
                        valueObject->setNull(true);
                        result->set(currentTokens[0], valueObject);
                    } else if (nextCount < count) {
                        // 当前行元素是 null
                        // 下一个元素不是本对象所属
                        auto valueObject = std::make_shared<BasicData>();
                        valueObject->setNull(true);
                        result->set(currentTokens[0], valueObject);
                        return result;
                    } else if (nextCount > count) {
                        // 新的数组
                        if (nextTokens[0] == "-") {
                            auto valueObject = createArray(tokensQueue, level - 1);
                            if (valueObject) {
                                result->set(currentTokens[0], valueObject);
                            } else {
                                return nullptr;
                            }
                        }
                        // 新的对象
                        else {
                            auto valueObject = createObject(tokensQueue, level - 1);
                            if (valueObject) {
                                result->set(currentTokens[0], valueObject);
                            } else {
                                return nullptr;
                            }
                        }
                    }
                }
            }
        } else if (count > currentCount) {
            // 当前对象结束
            return result;
        }
        // 错误 - 这几乎不可能发生
        // GCOVR_EXCL_START
        else {
            tokensQueue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return result;
}

sese::yaml::ArrayData::Ptr
sese::yaml::YamlUtil::createArray(
        sese::yaml::YamlUtil::TokensQueue &tokensQueue,
        size_t level
) noexcept {
    if (level == 0) return nullptr;

    auto result = std::make_shared<ArrayData>();
    // 对象无子元素
    if (tokensQueue.empty()) return result;
    int count = std::get<0>(tokensQueue.front());

    while (!tokensQueue.empty()) {
        decltype(auto) countAndTokens = tokensQueue.front();
        auto currentCount = std::get<0>(countAndTokens);
        auto currentTokens = std::get<1>(countAndTokens);

        if (count == currentCount) {
            if (currentTokens.size() == 1) {
                auto valueObject = std::make_shared<BasicData>();
                valueObject->setNull(true);
                result->push(valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 2) {
                std::string value = currentTokens[1];
                auto valueObject = std::make_shared<BasicData>();
                if (0 == strcasecmp(value.c_str(), "null") || value == "~") {
                    valueObject->setNull(true);
                } else {
                    valueObject->setDataAs<std::string>(value);
                }
                result->push(valueObject);
                tokensQueue.pop();
            } else if (currentTokens.size() == 3) {
                // 新的对象或数组
                auto currentCountAndTokens = tokensQueue.front();
                tokensQueue.pop();
                if (tokensQueue.empty()) {
                    // 不存在下一行，跳过该对象
                    return result;
                } else {
                    auto nextCountAndTokens = tokensQueue.front();
                    auto nextCount = std::get<0>(nextCountAndTokens);
                    auto nextTokens = std::get<1>(nextCountAndTokens);
                    if (nextCount > count) {
                        if (nextTokens[0] == "-") {
                            auto valueObject = createArray(tokensQueue, level - 1);
                            if (valueObject) {
                                result->push(valueObject);
                            } else {
                                return nullptr;
                            }
                        } else {
                            auto valueObject = createObject(tokensQueue, level - 1);
                            if (valueObject) {
                                result->push(valueObject);
                            } else {
                                return nullptr;
                            }
                        }
                    }
                    // 错误 - 这几乎不会发生
                    // GCOVR_EXCL_START
                    else {
                        tokensQueue.pop();
                        continue;
                    }
                    // GCOVR_EXCL_STOP
                }
            }
        } else if (count > currentCount) {
            // 当前数组结束
            return result;
        }
        // 错误 - 这几乎不会发生
        // GCOVR_EXCL_START
        else {
            tokensQueue.pop();
            continue;
        }
        // GCOVR_EXCL_STOP
    }
    return result;
}

constexpr auto getSpaceArray = []() {
    std::array<char, 1024> array{};
    for (decltype(auto) item: array) {
        item = ' ';
    }
    return array;
};

void sese::yaml::YamlUtil::writeSpace(size_t count, sese::OutputStream *output) noexcept {
    auto buffer = getSpaceArray();
    auto size = std::min<size_t>(1024, count);
    output->write(buffer.data(), size);
}

void sese::yaml::YamlUtil::serializeObject(ObjectData *objectData, sese::OutputStream *output, size_t level) noexcept {
    for (decltype(auto) item: *objectData) {
        if (item.second->getType() == DataType::ObjectData) {
            auto sub = dynamic_cast<ObjectData *>(item.second.get());// GCOVR_EXCL_LINE
            if (!sub->empty()) {
                writeSpace(level * 2, output);
                output->write(item.first.c_str(), item.first.length());
                output->write(":\n", 2);
                serializeObject(sub, output, level + 1);
            }
        } else if (item.second->getType() == DataType::ArrayData) {
            auto sub = dynamic_cast<ArrayData *>(item.second.get());// GCOVR_EXCL_LINE
            if (!sub->empty()) {
                writeSpace(level * 2, output);
                output->write(item.first.c_str(), item.first.length());
                output->write(":\n", 2);
                serializeArray(sub, output, level + 1);
            }
        } else {
            auto sub = dynamic_cast<BasicData *>(item.second.get());// GCOVR_EXCL_LINE
            auto data = sub->getDataAs<std::string>("");
            writeSpace(level * 2, output);
            output->write(item.first.c_str(), item.first.length());
            output->write(": ", 2);
            if (sub->isNull()) {
                output->write("~", 1);
            } else {
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}

void sese::yaml::YamlUtil::serializeArray(ArrayData *arrayData, sese::OutputStream *output, size_t level) noexcept {
    auto count = 0;
    for (decltype(auto) item: *arrayData) {
        if (item->getType() == DataType::ObjectData) {
            auto sub = dynamic_cast<ObjectData *>(item.get());// GCOVR_EXCL_LINE
            if (!sub->empty()) {                              // GCOVR_EXCL_LINE
                auto name = "element_" + std::to_string(count);
                count += 1;
                writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                serializeObject(sub, output, level + 1);
            }
        } else if (item->getType() == DataType::ArrayData) {
            auto sub = dynamic_cast<ArrayData *>(item.get());// GCOVR_EXCL_LINE
            if (!sub->empty()) {                             // GCOVR_EXCL_LINE
                auto name = "element_" + std::to_string(count);
                count += 1;
                writeSpace(level * 2, output);
                output->write("- ", 2);
                output->write(name.c_str(), name.length());
                output->write(":\n", 2);
                serializeArray(sub, output, level + 1);
            }
        } else {
            auto sub = dynamic_cast<BasicData *>(item.get());// GCOVR_EXCL_LINE
            auto data = sub->getDataAs<std::string>("");
            writeSpace(level * 2, output);
            output->write("- ", 2);
            if (sub->isNull()) {
                output->write("~", 1);
            } else {
                output->write(data.c_str(), data.length());
            }
            output->write("\n", 1);
        }
    }
}