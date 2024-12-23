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

/// \file Yaml.h
/// \brief YAML parser
/// \author Kaoru
/// \date November 4, 2023

#pragma once

#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>
#include <sese/util/Value.h>

#include <queue>
#include <stack>
#include <tuple>

namespace sese {
/// YAML parser
class Yaml final : public NotInstantiable {
    using Tokens = std::vector<std::string>;
    using TokensQueue = std::queue<std::tuple<int, Tokens>>;
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;
    using Line = std::tuple<int, std::string>;

    using ParseStack = std::stack<std::pair<Value *, int>>;
    using StreamifyStack = std::stack<std::tuple<Value *, unsigned int, unsigned int, bool>>;
    using StreamifyIterStack = std::stack<std::map<std::string, std::shared_ptr<Value>>::iterator>;

    static Value parseBasic(const std::string &value);

    static bool parseObject(
        TokensQueue &tokens_queue,
        ParseStack &stack
    );

    static bool parseArray(
        TokensQueue &tokens_queue,
        ParseStack &stack
    );

    static bool streamifyObject(
        OutputStream *output,
        StreamifyStack &stack,
        StreamifyIterStack &map_iter_stack
    );

    static bool streamifyArray(
        OutputStream *output,
        StreamifyStack &stack,
        StreamifyIterStack &map_iter_stack
    );

    static bool streamifyBasic(OutputStream *output, const std::shared_ptr<Value> &value);

    static int getSpaceCount(const std::string &line) noexcept;

    static Line getLine(InputStream *input) noexcept;

    static Tokens tokenizer(const std::string &line) noexcept;

    static bool writeSpace(OutputStream *output, size_t count) noexcept;

public:
    Yaml() = delete;

    /// Deserialize yaml object from stream
    /// \param input Input stream
    /// \return If parsing fails, Value::isNull is true
    static Value parse(io::InputStream *input);

    /// Serialize yaml object to stream
    /// \param output Output stream
    /// \param value yaml object
    /// \return If serialization fails, return false
    static bool streamify(io::OutputStream *output, Value &value);
};
} // namespace sese