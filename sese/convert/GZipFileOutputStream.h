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

/// \file GZipFileOutputStream.h
/// \author kaoru
/// \brief GZip file output stream (compression)
/// \version 0.1
/// \date March 15, 2023

#pragma once

#include "sese/io/OutputStream.h"

namespace sese {

/// GZip file output stream (compression)
class  GZipFileOutputStream : public io::OutputStream {
public:
    static void deleter(GZipFileOutputStream *data) noexcept;

    using Ptr = std::shared_ptr<GZipFileOutputStream>;

    /// Create file output stream object
    /// \param file Relative or absolute path of the file
    /// \param level Compression level (range 0-9, automatically handles value range)
    /// \retval nullptr Failed to open file
    static GZipFileOutputStream::Ptr create(const char *file, size_t level) noexcept;

    int64_t write(const void *buffer, size_t length) override;

    void close() noexcept;

private:
    GZipFileOutputStream() = default;

    void *gzFile = nullptr;
};

} // namespace sese