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

/// \file CSVWriter.h
/// \brief CSV stream writer
/// \author Kaoru
/// \version 0.1
/// \date March 30, 2023

#pragma once

#include "sese/io/OutputStream.h"

#include <vector>
#include <string>

namespace sese {

/// CSV stream writer
class  CSVWriter {
public:
    /// Output stream supported by the utility
    using OutputStream = io::OutputStream;
    using Row = std::vector<std::string>;

    static const char *crlf;
    static const char *lf;

    /// Constructor
    /// \param dest The destination stream
    /// \param split_char The delimiter character
    /// \param crlf Whether to use CRLF as the line separator
    explicit CSVWriter(OutputStream *dest, char split_char = ',', bool crlf = true) noexcept;

    /// Write to the stream
    /// \param row The row to be written
    void write(const Row &row) noexcept;

protected:
    char splitChar;
    bool isCRLF;
    OutputStream *dest = nullptr;
};
} // namespace sese