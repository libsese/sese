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

/// \file Json.h
/// \brief JSON parser
/// \author Kaoru
/// \date November 3, 2023

#pragma once

#include <sese/util/Value.h>
#include <sese/util/NotInstantiable.h>
#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

#include <stack>
#include <queue>

namespace simdjson::dom {
class element;
}

namespace sese {
/// JSON parser
class Json final : public NotInstantiable {
    using Tokens = std::queue<std::string>;

    static Value parse(Tokens &tokens);

    static bool parseObject(Tokens &tokens, std::stack<Value *> &stack);

    static bool parseArray(Tokens &tokens, std::stack<Value *> &stack);

    static Value parseBasic(const std::string &value);

    static bool streamifyObject(
        io::OutputStream *out,
        std::stack<std::pair<Value *, unsigned int>> &stack,
        std::stack<std::map<std::string, std::shared_ptr<Value>>::iterator> &map_iter_stack
    );

    static bool streamifyArray(
        io::OutputStream *out,
        std::stack<std::pair<Value *, unsigned int>> &stack,
        std::stack<std::map<std::string, std::shared_ptr<Value>>::iterator> &map_iter_stack
    );

    static bool streamifyBasic(io::OutputStream *out, const std::shared_ptr<Value> &value);

    static bool tokenizer(io::InputStream *input_stream, Tokens &tokens) noexcept;

    static Value parse(const simdjson::dom::element &json);

public:
    Json() = delete;

    /// \brief Deserialize a JSON object from the stream
    /// \param input The input stream
    /// \return If parsing fails, Value::isNull will be true
    static Value parse(io::InputStream *input);

    /// \brief Serialize a JSON object to the stream
    /// \param out The output stream
    /// \param value The JSON object
    /// \return If serialization fails, false will be returned
    static bool streamify(io::OutputStream *out, Value &value);

    /// \brief Deserialize a JSON object from the stream using SIMD
    /// \param input The input stream
    /// \return If parsing fails, Value::isNull will be true
    static Value simdParse(io::InputStream *input);
};
} // namespace sese