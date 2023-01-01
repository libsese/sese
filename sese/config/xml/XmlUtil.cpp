#include <sese/config/xml/XmlUtil.h>
#include "sese/util/BufferedStream.h"
#include <sese/text/StringBuilder.h>
#include "sese/util/Util.h"

namespace sese::xml {

    void XmlUtil::tokenizer(const InputStream::Ptr &inputStream, sese::xml::XmlUtil::Tokens &tokens) noexcept {
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
                if (tokens.front() == "<") {
                    if (!stringBuilder.empty())
                        tokens.push(stringBuilder.toString());
                    stringBuilder.clear();
                    tokens.push({ch});
                } else {
                    if (!stringBuilder.empty()) {
                        tokens.push(stringBuilder.toString());
                        stringBuilder.clear();
                    }
                    tokens.push({ch});
                }
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

    Element::Ptr XmlUtil::deserialize(const InputStream::Ptr &inputStream, size_t level) noexcept {
        // 此处懒得处理不合规格的格式，直接 catch
        Tokens tokens;
        tokenizer(inputStream, tokens);
        try {
            return createElement(tokens, level, false);
        } catch (...) {
            return nullptr;
        }
    }

    void XmlUtil::removeComment(sese::xml::XmlUtil::Tokens &tokens) noexcept {
        tokens.pop(); // "!--"
        while(tokens.front() != "--") {
            tokens.pop();
        }
        tokens.pop(); // "--"
        tokens.pop(); // ">"
    }

    Element::Ptr XmlUtil::createElement(sese::xml::XmlUtil::Tokens &tokens, size_t level, bool isSubElement) noexcept {
        if (level == 0) { return nullptr; }
        Element::Ptr element = nullptr;
        while (!tokens.empty()) {
            // <name(/,w)>
            if (!isSubElement) {
                if (tokens.front() != "<") return nullptr;
                tokens.pop();

                // 根节点注释
                if (tokens.front() == "!--") {
                    removeComment(tokens);
                    continue;
                }
            } else {
                if (tokens.front() == "!--") {
                    removeComment(tokens);
                    tokens.pop();// '<'
                }
            }

            element = std::make_shared<Element>(tokens.front());
            tokens.pop();

            // attrName=""
            if (tokens.front() != ">" && tokens.front() != "/") {
                while (tokens.front() != ">" && tokens.front() != "/") {
                    std::string attrName = tokens.front();
                    tokens.pop();// attrName
                    tokens.pop();// =
                    element->setAttribute(attrName, tokens.front());
                    tokens.pop();// attrValue
                }
            }

            // <name (attr="")/>
            if (tokens.front() == "/") {
                tokens.pop();// '/'
                tokens.pop();// '>'
                return element;
            } else if (tokens.front() == ">") {
                tokens.pop();// '>'
                // 子对象
                if (tokens.front() == "<") {
                    while (tokens.front() == "<") {
                        tokens.pop();// '<'
                        if (tokens.front() == "/") {
                            // 父对象结尾
                            tokens.pop();// '/'
                            tokens.pop();// name
                            tokens.pop();// '>'
                            return element;
                        } else {
                            level--;
                            auto object = createElement(tokens, level, true);
                            level++;
                            if (object) element->elements.push_back(object);
                        }
                    }
                } else {
                    std::string value = tokens.front();
                    tokens.pop();
                    while (tokens.front() != "<") {
                        value += " " + tokens.front();
                        tokens.pop();
                    }
                    element->setValue(value);

                    tokens.pop();// '<'
                    tokens.pop();// '/'
                    tokens.pop();// name
                    tokens.pop();// '>'
                    return element;
                }
            }
        }

        return element;
    }

    void XmlUtil::serialize(const Element::Ptr &object, const sese::OutputStream::Ptr &stream) noexcept {
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