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
 * @brief XML Serialization Utility Class
 * @author kaoru
 * @date October 23, 2022
 * @version 0.2
 */

#pragma once

#include "sese/config/xml/XmlTypes.h"
#include "sese/io/Stream.h"
#include "sese/util/NotInstantiable.h"

#include <queue>

namespace sese::xml {

/// XML Serialization Utility Class
class  XmlUtil final : public NotInstantiable {
public:
    using InputStream = io::InputStream;
    using OutputStream = io::OutputStream;
    using Tokens = std::queue<std::string>;

    XmlUtil() = delete;

    /// Deserialize an XML element object from a stream
    /// \param input_stream Input stream
    /// \param level Deserialization depth
    /// \retval nullptr Deserialization failed, otherwise succeeded
    static Element::Ptr deserialize(const InputStream::Ptr &input_stream, size_t level) noexcept;

    /// Deserialize an XML element object from a stream
    /// \param input_stream Input stream
    /// \param level Deserialization depth
    /// \retval nullptr Deserialization failed, otherwise succeeded
    static Element::Ptr deserialize(InputStream *input_stream, size_t level) noexcept;

    /// Serialize an XML element object to a stream
    /// \param object Object to be serialized
    /// \param output_stream Output stream
    static void serialize(const Element::Ptr &object, const OutputStream::Ptr &output_stream) noexcept;

    /// Serialize an XML element object to a stream
    /// \param object Object to be serialized
    /// \param output_stream Output stream
    static void serialize(const Element::Ptr &object, OutputStream *output_stream) noexcept;

private:
    static void tokenizer(InputStream *input_stream, Tokens &tokens) noexcept;

    static bool removeComment(Tokens &tokens) noexcept;

    static Element::Ptr createElement(Tokens &tokens, size_t level, bool is_sub_element) noexcept;
};
} // namespace sese::xml