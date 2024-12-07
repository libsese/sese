// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/config/xml/XmlUtil.h>
#include <sese/io/BufferedStream.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

namespace sese::xml {

Element::Ptr XmlUtil::deserialize(const InputStream::Ptr &input_stream, size_t level) noexcept {
    return deserialize(input_stream.get(), level);
}

void XmlUtil::serialize(const Element::Ptr &object, const OutputStream::Ptr &output_stream) noexcept {
    serialize(object, output_stream.get());
}

void XmlUtil::tokenizer(InputStream *input_stream, sese::xml::XmlUtil::Tokens &tokens) noexcept {
    text::StringBuilder string_builder;
    char ch;
    int64_t len;
    while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
        if (sese::isSpace(ch)) {
            if (string_builder.empty()) continue;
            tokens.push(string_builder.toString());
            string_builder.clear();
        } else if (ch == '>') {
            if (!string_builder.empty())
                tokens.push(string_builder.toString());
            tokens.push({ch});
            string_builder.clear();
        } else if (ch == '<') {
            if (!string_builder.empty()) {
                tokens.push(string_builder.toString());
                string_builder.clear();
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
            if (!string_builder.empty()) {
                tokens.push(string_builder.toString());
                string_builder.clear();
            }
            tokens.push({ch});
        } else if (ch == '=') {
            tokens.push(string_builder.toString());
            string_builder.clear();
            tokens.push({ch});
        } else if (ch == '\"') {
            while ((len = input_stream->read(&ch, 1 * sizeof(char))) != 0) {
                if (ch == '\"') {
                    tokens.push(string_builder.toString());
                    string_builder.clear();
                    break;
                } else {
                    string_builder.append(ch);
                }
            }
        } else {
            string_builder.append(ch);
        }
    }
}

Element::Ptr XmlUtil::deserialize(InputStream *input_stream, size_t level) noexcept {
    Tokens tokens;
    tokenizer(input_stream, tokens);
    return createElement(tokens, level, false);
}

bool XmlUtil::removeComment(sese::xml::XmlUtil::Tokens &tokens) noexcept {
    tokens.pop(); // "!--"
    while (!tokens.empty()) {
        if (tokens.front() == "--") {
            break;
        } else {
            tokens.pop();
        }
    }
    if (tokens.empty()) return false; // GCOVR_EXCL_LINE
    // if (tokens.front() != "--") return false;
    // Execute to this point tokens.front() is "--", no need to judge
    tokens.pop(); // "--"

    if (tokens.empty()) return false; // GCOVR_EXCL_LINE
    if (tokens.front() != ">") return false;
    tokens.pop(); // ">"
    return true;
}

#define CHECK(word)                     \
    if (tokens.empty()) return nullptr; \
    if (tokens.front() != word) return nullptr

Element::Ptr XmlUtil::createElement(sese::xml::XmlUtil::Tokens &tokens, size_t level, bool is_sub_element) noexcept {
    if (level == 0) return nullptr;
    Element::Ptr element = nullptr;
    while (!tokens.empty()) {
        // <name(/,w)>
        if (!is_sub_element) {
            if (tokens.front() != "<") return nullptr;
            tokens.pop();

            if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
            // root node comment
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
                    tokens.pop(); // '<'
                }
            }
        }

        element = std::make_shared<Element>(tokens.front());
        tokens.pop();

        if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
        // attrName=""
        while (tokens.front() != ">" && tokens.front() != "/") {
            std::string attr_name = tokens.front();
            tokens.pop(); // attrName
            // if (tokens.empty()) return nullptr;
            // if (tokens.front() != "=") return nullptr;
            CHECK("=");   // GCOVR_EXCL_LINE
            tokens.pop(); // =

            if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
            element->setAttribute(attr_name, tokens.front());
            tokens.pop(); // attrValue
        }

        if (tokens.empty()) return nullptr; // GCOVR_EXCL_LINE
        // <name (attr="")/>
        if (tokens.front() == "/") {
            tokens.pop(); // '/'
            // if (tokens.empty()) return nullptr;
            // if (tokens.front() != ">") return nullptr;
            CHECK(">");   // GCOVR_EXCL_LINE
            tokens.pop(); // '>'
            return element;
        } else if (tokens.front() == ">") {
            tokens.pop(); // '>'
            if (tokens.empty()) return nullptr;
            // sub element
            if (tokens.front() == "<") {
                while (tokens.front() == "<") {
                    tokens.pop(); // '<'
                    if (tokens.front() == "/") {
                        // end of parent object
                        tokens.pop(); // '/'
                        // if (tokens.empty()) return nullptr;
                        // if (tokens.front() != element->getName()) return nullptr;
                        CHECK(element->getName()); // GCOVR_EXCL_LINE
                        tokens.pop();              // name
                        // if (tokens.empty()) return nullptr;
                        // if (tokens.front() != ">") return nullptr;
                        CHECK(">");   // GCOVR_EXCL_LINE
                        tokens.pop(); // '>'
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
                text::StringBuilder string_builder;
                while (!tokens.empty()) {
                    if (tokens.front() == "<") {
                        break;
                    } else {
                        if (first) {
                            first = false;
                        } else {
                            string_builder.append(" ");
                        }
                        string_builder.append(tokens.front());
                        tokens.pop();
                    }
                }
                element->setValue(string_builder.toString());

                CHECK("<");                // GCOVR_EXCL_LINE
                tokens.pop();              // '<'
                CHECK("/");                // GCOVR_EXCL_LINE
                tokens.pop();              // '/'
                CHECK(element->getName()); // GCOVR_EXCL_LINE
                tokens.pop();              // name
                CHECK(">");                // GCOVR_EXCL_LINE
                tokens.pop();              // '>'
                return element;
            }
        }
    }
    return element;
}

void XmlUtil::serialize(const Element::Ptr &object, OutputStream *stream) noexcept {
    const auto NAME = object->getName();
    const auto ATTRIBUTES = object->getAttributes();
    const auto VALUE = object->getValue();
    stream->write("<", 1);
    stream->write(NAME.c_str(), NAME.length());

    if (!ATTRIBUTES.empty()) {
        for (decltype(auto) attr: ATTRIBUTES) {
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
        stream->write(NAME.c_str(), NAME.length());
        stream->write(">", 1);
    } else {
        if (VALUE.empty()) {
            stream->write("/>", 2);
        } else {
            stream->write(">", 1);
            stream->write(VALUE.c_str(), VALUE.length());
            stream->write("</", 2);
            stream->write(NAME.c_str(), NAME.length());
            stream->write(">", 1);
        }
    }
}

} // namespace sese::xml