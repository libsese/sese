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
/// \brief Json 解析器
/// \author kaoru
/// \date 2023年11月3日

#pragma once

#include <sese/util/Value.h>
#include <sese/util/NotInstantiable.h>
#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

#include <queue>

namespace sese {
/// Json 解析器
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

    /// \brief 从流中反序列化 Json 对象
    /// \param input 输入流
    /// \param level Json 解析深度
    /// \return 解析失败则 Value::isNull 为真
    static Value parse(io::InputStream *input, size_t level);

    /// \brief 序列化 Json 对象至流中
    /// \param out 输出流
    /// \param dict Json 对象
    static void streamify(io::OutputStream *out, const Value::Dict &dict);
};
} // namespace sese