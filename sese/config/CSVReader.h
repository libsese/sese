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

/// \file CSVReader.h
/// \brief CSV stream reader
/// \author Kaoru
/// \version 0.1
/// \date March 30, 2023

#pragma once

#include "sese/io/InputStream.h"

#include <vector>
#include <string>

namespace sese {

/// CSV stream reader
class  CSVReader {
public:
    /// Input stream supported by the utility
    using InputStream = io::InputStream;
    using Row = std::vector<std::string>;

    /// Constructor
    /// \param source The input stream to read from
    /// \param split_char The delimiter character
    explicit CSVReader(InputStream *source, char split_char = ',') noexcept;

    /// Reads a line from the stream
    /// \return A vector container containing all elements in the line
    Row read() noexcept;

protected:
    char splitChar;
    InputStream *source = nullptr;
};
} // namespace sese
