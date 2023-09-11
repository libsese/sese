#include <sese/config/xml/XmlUtil.h>
#include <sese/io/BufferedStream.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

namespace sese::xml {

    Element::Ptr XmlUtil::deserialize(const InputStream::Ptr &inputStream, size_t level) noexcept {
        return deserialize(inputStream.get(), level);
    }

    void XmlUtil::serialize(const Element::Ptr &object, const OutputStream::Ptr &outputStream) noexcept {
        serialize(object, outputStream.get());
    }

    void XmlUtil::tokenizer(InputStream *inputStream, sese::xml::XmlUtil::Tokens &tokens) noexcept {
        text::StringBuilder stringBuilder;
        char ch;
        int64_t len;
        while ((len = inputStream->read(&ch, 1 * sizeof(char))) != 0) {
            if (sese::isSpace(ch)) {
                if (stringBuilder.empty()) continue;
                tokens.push(stringBuilder.toString());
                stringBuilder.clear();
            } else if (ch == '>') {
                if (!stringBuilder.empty())
                    tokens.push(stringBuilder.toString());
                tokens.push({ch});
                stringBuilder.clear();
            } else if (ch == '<') {
                if (!stringBuilder.empty()) {
                    tokens.push(stringBuilder.toString());
                    stringBuilder.clear();
                }
                tokens.push({ch});

            } else if (ch == '/') {
                // if (tokens.front() == "<") {
                //     if (!stringBuilder.empty()) {
                //         tokens.push(stringBuilder.toString());
                //         stringBuilder.clear();
                //     }
                //     tokens.push({ch});
                // } else {
                //     if (!stringBuilder.empty()) {
                //         tokens.push(stringBuilder.toString());
                //         stringBuilder.clear();
                //     }
                //     tokens.push({ch});
                // }
                if (!stringBuilder.empty()) {
                    tokens.push(stringBuilder.toString());
                    stringBuilder.clear();
                }
                tokens.push({ch});
            } else if (ch == '=') {
                tokens.push(stringBuilder.toString());
                stringBuilder.clear();
                tokens.push({ch});
            } else if (ch == '\"') {
                while ((len = inputStream->read(&ch, 1 * sizeof(char))) != 0) {
                    if (ch == '\"') {
                        tokens.push(stringBuilder.toString());
                        stringBuilder.clear();
                        break;
                    } else {
                        stringBuilder.append(ch);
                    }
                }
            } else {
                stringBuilder.append(ch);
            }
        }
    }

    Element::Ptr XmlUtil::deserialize(InputStream *inputStream, size_t level) noexcept {
        Tokens tokens;
        tokenizer(inputStream, tokens);
        return createElement(tokens, level, false);
    }

    bool XmlUtil::removeComment(sese::xml::XmlUtil::Tokens &tokens) noexcept {
        tokens.pop();// "!--"
        while (!tokens.empty()) {
            if (tokens.front() == "--") {
                break;
            } else {
                tokens.pop();
            }
        }
        if (tokens.empty()) return false;// GCOVR_EXCL_LINE
        // if (tokens.front() != "--") return false;
        // 能执行到此处 tokens.front() 一定是 "--"，不需要判断
        tokens.pop();// "--"

        if (tokens.empty()) return false;// GCOVR_EXCL_LINE
        if (tokens.front() != ">") return false;
        tokens.pop();// ">"
        return true;
    }

#define CHECK(word)                     \
    if (tokens.empty()) return nullptr; \
    if (tokens.front() != word) return nullptr

    Element::Ptr XmlUtil::createElement(sese::xml::XmlUtil::Tokens &tokens, size_t level, bool isSubElement) noexcept {
        if (level == 0) return nullptr;
        Element::Ptr element = nullptr;
        while (!tokens.empty()) {
            // <name(/,w)>
            if (!isSubElement) {
                if (tokens.front() != "<") return nullptr;
                tokens.pop();

                if (tokens.empty()) return nullptr;// GCOVR_EXCL_LINE
                // 根节点注释
                if (tokens.front() == "!--") {
                    if (!removeComment(tokens)) {
                        return nullptr;
                    } else {
                        continue;
                    }
                }
            } else {
                if (tokens.front() == "!--") {
                    if (!removeComment(tokens)) {
                        return nullptr;
                    } else {
                        tokens.pop();// '<'
                    }
                }
            }

            element = std::make_shared<Element>(tokens.front());
            tokens.pop();

            if (tokens.empty()) return nullptr;// GCOVR_EXCL_LINE
            // attrName=""
            while (tokens.front() != ">" && tokens.front() != "/") {
                std::string attrName = tokens.front();
                tokens.pop();// attrName
                // if (tokens.empty()) return nullptr;
                // if (tokens.front() != "=") return nullptr;
                CHECK("=");  // GCOVR_EXCL_LINE
                tokens.pop();// =

                if (tokens.empty()) return nullptr;// GCOVR_EXCL_LINE
                element->setAttribute(attrName, tokens.front());
                tokens.pop();// attrValue
            }

            if (tokens.empty()) return nullptr;// GCOVR_EXCL_LINE
            // <name (attr="")/>
            if (tokens.front() == "/") {
                tokens.pop();// '/'
                // if (tokens.empty()) return nullptr;
                // if (tokens.front() != ">") return nullptr;
                CHECK(">");  // GCOVR_EXCL_LINE
                tokens.pop();// '>'
                return element;
            } else if (tokens.front() == ">") {
                tokens.pop();// '>'
                if (tokens.empty()) return nullptr;
                // 子对象
                if (tokens.front() == "<") {
                    while (tokens.front() == "<") {
                        tokens.pop();// '<'
                        if (tokens.front() == "/") {
                            // 父对象结尾
                            tokens.pop();// '/'
                            // if (tokens.empty()) return nullptr;
                            // if (tokens.front() != element->getName()) return nullptr;
                            CHECK(element->getName());// GCOVR_EXCL_LINE
                            tokens.pop();             // name
                            // if (tokens.empty()) return nullptr;
                            // if (tokens.front() != ">") return nullptr;
                            CHECK(">");  // GCOVR_EXCL_LINE
                            tokens.pop();// '>'
                            return element;
                        } else {
                            level--;
                            auto object = createElement(tokens, level, true);
                            level++;
                            if (object) {
                                element->elements.push_back(object);
                            } else {
                                return nullptr;
                            }
                        }
                    }
                } else {
                    bool first = true;
                    text::StringBuilder stringBuilder;
                    while (!tokens.empty()) {
                        if (tokens.front() == "<") {
                            break;
                        } else {
                            if (first) {
                                first = false;
                            } else {
                                stringBuilder.append(" ");
                            }
                            stringBuilder.append(tokens.front());
                            tokens.pop();
                        }
                    }
                    element->setValue(stringBuilder.toString());

                    CHECK("<");               // GCOVR_EXCL_LINE
                    tokens.pop();             // '<'
                    CHECK("/");               // GCOVR_EXCL_LINE
                    tokens.pop();             // '/'
                    CHECK(element->getName());// GCOVR_EXCL_LINE
                    tokens.pop();             // name
                    CHECK(">");               // GCOVR_EXCL_LINE
                    tokens.pop();             // '>'
                    return element;
                }
            }
        }
        return element;
    }

    void XmlUtil::serialize(const Element::Ptr &object, OutputStream *stream) noexcept {
        auto name = object->getName();
        auto attributes = object->getAttributes();
        auto value = object->getValue();
        stream->write("<", 1);
        stream->write(name.c_str(), name.length());

        if (!attributes.empty()) {
            for (decltype(auto) attr: attributes) {
                stream->write(" ", 1);
                stream->write(attr.first.c_str(), attr.first.length());
                stream->write("=\"", 2);
                stream->write(attr.second.c_str(), attr.second.length());
                stream->write("\"", 1);
            }
        }

        if (!object->elements.empty()) {
            stream->write(">", 1);
            for (decltype(auto) element: object->elements) {
                serialize(element, stream);
            }
            stream->write("</", 2);
            stream->write(name.c_str(), name.length());
            stream->write(">", 1);
        } else {
            if (value.length() == 0) {
                stream->write("/>", 2);
            } else {
                stream->write(">", 1);
                stream->write(value.c_str(), value.length());
                stream->write("</", 2);
                stream->write(name.c_str(), name.length());
                stream->write(">", 1);
            }
        }
    }

}// namespace sese::xml