/**
 * @file JsonUtil.h
 * @brief Json 序列化工具类
 * @author kaoru
 * @date 2022年5月14日
 */
#pragma once
#include <sese/config/json/JsonTypes.h>
#include "sese/util/Stream.h"
#include "sese/util/NotInstantiable.h"
#include <queue>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese::json {

    /// Json 序列化工具类
    class API JsonUtil : NotInstantiable {
    public:
        using Tokens = std::queue<std::string>;

    public:
        /**
         * 从流中反序列化 Json 对象
         * @param inputStream 输入流
         * @param level Json 解析深度
         * @return 解析得到的 JsonObject，解析失败时返回 nullptr（比如超过设定的解析深度，或者不合法的 token 排序，暂时不支持 \\u 转义）
         */
        static ObjectData::Ptr deserialize(const InputStream::Ptr &inputStream, size_t level) noexcept;
        /**
         * 序列化 Json 对象至流中
         * @param object Json 对象
         * @param outputStream 输出流
         */
        static void serialize(const ObjectData::Ptr &object, const OutputStream::Ptr &outputStream) noexcept;

    private:
        static bool tokenizer(const InputStream::Ptr &inputStream, Tokens &tokens) noexcept;

        static ObjectData::Ptr createObject(Tokens &tokens, size_t level) noexcept;
        static ArrayData::Ptr createArray(Tokens &tokens, size_t level) noexcept;

        static void serializeObject(const ObjectData::Ptr &object, const OutputStream::Ptr &outputStream) noexcept;
        static void serializeArray(const ArrayData::Ptr &array, const OutputStream::Ptr &outputStream) noexcept;
    };
}// namespace sese::json