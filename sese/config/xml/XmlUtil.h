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

/**
* @file XmlUtil.h
* @brief XML 序列化工具类
* @author kaoru
* @date 2022年10月23日
* @version 0.2
*/

#pragma once

#include "sese/config/xml/XmlTypes.h"
#include "sese/io/Stream.h"
#include "sese/util/NotInstantiable.h"

#include <queue>

namespace sese::xml {

/// XML 序列化工具类
class  XmlUtil final : public NotInstantiable {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;
    using Tokens = std::queue<std::string>;

    XmlUtil() = delete;

    /// 从流中反序列化一个 Xml 元素对象
    /// \param input_stream 输入流
    /// \param level 反序列化深度
    /// \retval nullptr 反序列化失败，否则为成功
    static Element::Ptr deserialize(const InputStream::Ptr &input_stream, size_t level) noexcept;

    /// 从流中反序列化一个 Xml 元素对象
    /// \param input_stream 输入流
    /// \param level 反序列化深度
    /// \retval nullptr 反序列化失败，否则为成功
    static Element::Ptr deserialize(InputStream *input_stream, size_t level) noexcept;

    /// 向流中序列化一个 Xml 元素对象
    /// \param object 待序列化对象
    /// \param output_stream 待输出流
    static void serialize(const Element::Ptr &object, const OutputStream::Ptr &output_stream) noexcept;

    /// 向流中序列化一个 Xml 元素对象
    /// \param object 待序列化对象
    /// \param output_stream 待输出流
    static void serialize(const Element::Ptr &object, OutputStream *output_stream) noexcept;

private:
    static void tokenizer(InputStream *input_stream, Tokens &tokens) noexcept;

    static bool removeComment(Tokens &tokens) noexcept;

    static Element::Ptr createElement(Tokens &tokens, size_t level, bool is_sub_element) noexcept;
};
} // namespace sese::xml