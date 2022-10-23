#pragma once
#include <sese/config/xml/XmlTypes.h>
#include <sese/Stream.h>
#include <sese/NotInstantiable.h>
#include <queue>

namespace sese::xml {

    class API XmlUtil : public NotInstantiable {
    public:
        using Tokens = std::queue<std::string>;

        static Element::Ptr deserialize(const Stream::Ptr &inputStream, size_t level) noexcept;

        static void serialize(const Element::Ptr &object, const Stream::Ptr &outputStream) noexcept;

    private:
        static void tokenizer(const Stream::Ptr &inputStream, Tokens &tokens) noexcept;

        static Element::Ptr createElement(Tokens &tokens, size_t level, bool isSubElement) noexcept;

        static void serialize(const Element::Ptr &object, Stream &stream);
    };
}// namespace sese::xml