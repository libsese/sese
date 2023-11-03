/**
 * @file JsonUtil.h
 * @brief JSON 序列化工具类
 * @author kaoru
 * @date 2022年5月14日
 * @version 0.2
 */
#pragma once

#include "sese/config/json/JsonTypes.h"
#include "sese/io/Stream.h"
#include "sese/util/NotInstantiable.h"

#include <queue>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::json {

/// JSON 序列化工具类
class API JsonUtil : public NotInstantiable {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;
    using Tokens = std::queue<std::string>;

    /**
     * 从流中反序列化 Json 对象
     * @param inputStream 输入流
     * @param level Json 解析深度
     * @return 解析得到的 JsonObject，解析失败时返回 nullptr（比如超过设定的解析深度，或者不合法的 token 排序，暂时不支持 \\u 转义）
     */
    static ObjectData::Ptr deserialize(const InputStream::Ptr &inputStream, size_t level) noexcept;

    /**
     * 从流中反序列化 Json 对象
     * @param inputStream 输入流
     * @param level Json 解析深度
     * @return 解析得到的 JsonObject，解析失败时返回 nullptr（比如超过设定的解析深度，或者不合法的 token 排序，暂时不支持 \\u 转义）
     */
    static ObjectData::Ptr deserialize(InputStream *inputStream, size_t level) noexcept;

    /**
     * 序列化 Json 对象至流中
     * @param object Json 对象
     * @param outputStream 输出流
     */
    static void serialize(const ObjectData::Ptr &object, const OutputStream::Ptr &outputStream) noexcept;

    /**
     * 序列化 Json 对象至流中
     * @param object Json 对象
     * @param outputStream 输出流
     */
    static void serialize(ObjectData *object, OutputStream *outputStream) noexcept;

    static bool tokenizer(InputStream *inputStream, Tokens &tokens) noexcept;

private:
    static ObjectData::Ptr createObject(Tokens &tokens, size_t level) noexcept;
    static ArrayData::Ptr createArray(Tokens &tokens, size_t level) noexcept;

    static void serializeObject(ObjectData *object, OutputStream *outputStream) noexcept;
    static void serializeArray(ArrayData *array, OutputStream *outputStream) noexcept;
};
} // namespace sese::json