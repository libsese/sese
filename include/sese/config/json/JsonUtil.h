#pragma once
#include <sese/config/json/JsonTypes.h>
#include <sese/Stream.h>
#include <queue>

namespace sese::json {

    class API JsonUtil {
    public:
        using Tokens = std::queue<std::string>;

    public:
        static bool tokenizer(const Stream::Ptr &inputStream, Tokens &tokens) noexcept;
        static ObjectData::Ptr parser(Tokens &tokens) noexcept;

    private:
        static ObjectData::Ptr  createObject(Tokens &tokens) noexcept;
        static ArrayData::Ptr createArray(Tokens &tokens) noexcept;
    };
}// namespace sese::json