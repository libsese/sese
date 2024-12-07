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
 * \file IniUtil.h
 * \author kaoru
 * \brief INI Utility Class
 * \date September 13, 2023
 */

#pragma once

#include <sese/config/ini/IniConfig.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

namespace sese::ini {

/// INI Utility Class
class  IniUtil {
public:
    using InputStream = io::PeekableStream;
    using OutputStream = io::OutputStream;
    using IniConfigObject = std::unique_ptr<IniConfig>;

    /**
     * Parse an INI CONFIG object from a peekable stream
     * @param input Peekable stream to be processed
     * @return INI CONFIG object
     */
    static IniConfigObject parse(InputStream *input) noexcept;

    /**
     * Write an INI CONFIG object to a stream
     * @param target INI CONFIG to be written
     * @param output Stream to output
     * @return Whether the operation is successful
     */
    static bool unparse(const IniConfigObject &target, OutputStream *output) noexcept;

protected:
    static bool unparsePair(const std::pair<std::string, std::string> &pair, OutputStream *output) noexcept;
};

} // namespace sese::ini