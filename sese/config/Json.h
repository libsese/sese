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

#include <queue>

namespace sese {
/// JSON parser
class Json final : public NotInstantiable {
    using Tokens = std::queue<std::string>;

    static Value parseObject(Tokens &tokens, size_t level);

    static Value parseArray(Tokens &tokens, size_t level);

    static Value parseBasic(const std::string &value);

    static void streamifyObject(io::OutputStream *out, const Value::Dict &object);

    static void streamifyArray(io::OutputStream *out, const Value::List &array);

    static void streamifyBasic(io::OutputStream *out, const Value *value);

    static bool tokenizer(io::InputStream *input_stream, Tokens &tokens) noexcept;
public:
    Json() = delete;

    /// \brief Deserialize a JSON object from the stream
    /// \param input The input stream
    /// \param level The JSON parsing depth
    /// \return If parsing fails, Value::isNull will be true
    static Value parse(io::InputStream *input, size_t level);

    /// \brief Serialize a JSON object to the stream
    /// \param out The output stream
    /// \param dict The JSON object
    static void streamify(io::OutputStream *out, const Value::Dict &dict);
};
} // namespace sese