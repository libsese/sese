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

    Value root_value;
    if (json.type() == element_type::OBJECT) {
        root_value = Value::dict();
    } else if (json.type() == element_type::ARRAY) {
        root_value = Value::list();
    } else {
        return {};
    }
    stack.emplace(json, &root_value);

    while (!stack.empty()) {
        auto [element, currentValue] = stack.top();
        stack.pop();
        if (element.type() == element_type::ARRAY) {
            auto arr = element.get_array();
            auto &list_ref = currentValue->getList();
            for (auto child: arr) {
                switch (child.type()) {
                    case element_type::ARRAY: {
                        auto v = list_ref.appendRef(Value::list()).get();
                        stack.emplace(child, v);
                        break;
                    }
                    case element_type::OBJECT: {
                        auto v = list_ref.appendRef(Value::dict()).get();
                        stack.emplace(child, v);
                        break;
                    }
                    case element_type::INT64: {
                        list_ref.append(child.get_int64());
                        break;
                    }
                    case element_type::UINT64: {
                        list_ref.append(static_cast<int64_t>(child.get_uint64()));
                        break;
                    }
                    case element_type::DOUBLE: {
                        list_ref.append(child.get_double());
                        break;
                    }
                    case element_type::STRING: {
                        std::string str(child.get_string().take_value());
                        list_ref.append(std::move(str));
                        break;
                    }
                    case element_type::BOOL: {
                        list_ref.append(child.get_bool().value());
                        break;
                    }
                    case element_type::NULL_VALUE: {
                        list_ref.append(Value());
                        break;
                    }
                }
            }
        } else if (element.type() == element_type::OBJECT) {
            auto obj = element.get_object();
            auto &dict_ref = currentValue->getDict();
            for (auto &[_key, value]: obj) {
                std::string key(_key.begin(), _key.end());
                switch (value.type()) {
                    case element_type::ARRAY: {
                        auto v = dict_ref.setRef(key, Value::list()).get();
                        stack.emplace(value, v);
                        break;
                    }
                    case element_type::OBJECT: {
                        auto v = dict_ref.setRef(key, Value::dict()).get();
                        stack.emplace(value, v);
                        break;
                    }
                    case element_type::INT64: {
                        dict_ref.set(key, value.get_int64());
                        break;
                    }
                    case element_type::UINT64: {
                        dict_ref.set(key, static_cast<int64_t>(value.get_uint64()));
                        break;
                    }
                    case element_type::DOUBLE: {
                        dict_ref.set(key, value.get_double());
                        break;
                    }
                    case element_type::STRING: {
                        std::string str(value.get_string().take_value());
                        dict_ref.set(key, std::move(str));
                        break;
                    }
                    case element_type::BOOL: {
                        dict_ref.set(key, value.get_bool());
                        break;
                    }
                    case element_type::NULL_VALUE: {
                        dict_ref.set(key, std::move(Value()));
                        break;
                    }
                }
            }
        } else {
            assert(false);
        }
    }
    return root_value;
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