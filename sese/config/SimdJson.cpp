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

#include "Json.h"
#include <sese/text/StringBuilder.h>

#include <simdjson.h>

#include <stack>

using sese::Json;

sese::Value Json::parse(const simdjson::dom::element &json) {
    using simdjson::dom::element_type;
    std::stack<std::pair<simdjson::dom::element, Value *>> stack;
    auto rootValue = Value::dict();
    stack.push({json, &rootValue});

    while (!stack.empty()) {
        auto [element, currentValue] = stack.top();
        stack.pop();
        if (element.type() == element_type::ARRAY) {
            auto arr = element.get_array();
            auto &listRef = currentValue->getList();
            for (auto child: arr) {
                switch (child.type()) {
                    case element_type::ARRAY: {
                        auto v = listRef.appendRef(Value::list()).get();
                        stack.push({child, v});
                        break;
                    }
                    case element_type::OBJECT: {
                        auto v = listRef.appendRef(Value::dict()).get();
                        stack.push({child, v});
                        break;
                    }
                    case element_type::INT64: {
                        listRef.append(child.get_int64());
                        break;
                    }
                    case element_type::UINT64: {
                        listRef.append(static_cast<int64_t>(child.get_uint64()));
                        break;
                    }
                    case element_type::DOUBLE: {
                        listRef.append(child.get_double());
                        break;
                    }
                    case element_type::STRING: {
                        std::string str(child.get_string().take_value());
                        listRef.append(std::move(str));
                        break;
                    }
                    case element_type::BOOL: {
                        listRef.append(child.get_bool().value());
                        break;
                    }
                    case element_type::NULL_VALUE: {
                        listRef.append(Value());
                        break;
                    }
                }
            }
        } else if (element.type() == element_type::OBJECT) {
            auto obj = element.get_object();
            auto &dictRef = currentValue->getDict();
            for (auto &[_key, value]: obj) {
                std::string key(_key.begin(), _key.end());
                switch (value.type()) {
                    case element_type::ARRAY: {
                        auto v = dictRef.setRef(key, Value::list()).get();
                        stack.push({value, v});
                        break;
                    }
                    case element_type::OBJECT: {
                        auto v = dictRef.setRef(key, Value::dict()).get();
                        stack.push({value, v});
                        break;
                    }
                    case element_type::INT64: {
                        dictRef.set(key, value.get_int64());
                        break;
                    }
                    case element_type::UINT64: {
                        dictRef.set(key, static_cast<int64_t>(value.get_uint64()));
                        break;
                    }
                    case element_type::DOUBLE: {
                        dictRef.set(key, value.get_double());
                        break;
                    }
                    case element_type::STRING: {
                        std::string str(value.get_string().take_value());
                        dictRef.set(key, std::move(str));
                        break;
                    }
                    case element_type::BOOL: {
                        dictRef.set(key, value.get_bool());
                        break;
                    }
                    case element_type::NULL_VALUE: {
                        dictRef.set(key, std::move(Value()));
                        break;
                    }
                }
            }
        } else {
            assert(false);
        }
    }
    return rootValue;
}

sese::Value Json::simdParse(io::InputStream *input) {
    text::StringBuilder builder(1024 * 4);
    char buffer[1024 * 4];
    size_t read;
    while ((read = input->read(buffer, sizeof(buffer))) > 0) {
        builder.append(buffer, read);
    }
    memset(buffer, 0, simdjson::SIMDJSON_PADDING);
    builder.append(buffer, simdjson::SIMDJSON_PADDING);
    simdjson::padded_string_view padded_string(static_cast<const char *>(builder.buf()), builder.length());

    simdjson::dom::element json;
    simdjson::dom::parser parser;
    auto result = parser.parse(padded_string);
    if (result.error() || result.get(json)) {
        return {};
    }

    return parse(json);
}