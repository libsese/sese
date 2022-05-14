#pragma once
#include <sese/config/json/JsonTypes.h>
#include <sese/Stream.h>
#include <queue>

namespace sese::json {

    class API JsonUtil {
    public:
        using Tokens = std::queue<std::string>;

    public:
        /**
         * 从流中反序列化 Json 对象
         * @param inputStream 输入流
         * @param level Json 解析深度
         * @return 解析得到的 JsonObject，解析失败时返回 nullptr（比如超过设定的解析深度，或者不合法的 token 排序，暂时不支持 \\u 转义）
         */
        static ObjectData::Ptr deserialize(const Stream::Ptr &inputStream, size_t level) noexcept;
        /**
         * 序列化 Json 对象至流中
         * @param object Json 对象
         * @param outputStream 输出流
         */
        static void serialize(const ObjectData::Ptr &object, const Stream::Ptr &outputStream) noexcept;

    private:
        static bool tokenizer(const Stream::Ptr &inputStream, Tokens &tokens) noexcept;

        static ObjectData::Ptr createObject(Tokens &tokens, size_t level) noexcept;
        static ArrayData::Ptr createArray(Tokens &tokens, size_t level) noexcept;

        static void serializeObject(const ObjectData::Ptr &object, const Stream::Ptr &outputStream) noexcept;
        static void serializeArray(const ArrayData::Ptr &array, const Stream::Ptr &outputStream) noexcept;
    };
}// namespace sese::json