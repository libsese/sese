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

/// \file ArchiveReader.h
/// \brief Archive reader
/// \author Kaoru
/// \date January 9, 2024

#pragma once

#include <sese/archive/Config.h>
#include <sese/io/InputStream.h>

#include <functional>
#include <filesystem>

namespace sese::archive {

    /// Archive reader
    class ArchiveReader {
    public:
        using ExtractCallback = std::function<bool(const std::filesystem::path &base_path, Config::EntryType type, io::InputStream *input, size_t readable)>;

        /// Extracts the archive to the specified location
        /// \param src_path The path of the archive
        /// \param dest_path The destination path for extraction
        /// \param pwd The password (optional)
        /// \return Whether the operation was successful
        static bool extract(const std::filesystem::path &src_path, const std::filesystem::path &dest_path, const std::string &pwd = {});

        /// Reads the archive from the input stream
        /// \param input The input stream
        explicit ArchiveReader(io::InputStream *input);

        virtual ~ArchiveReader();

        /// Sets the password for the currently read document
        /// \param pwd The password to be used
        /// \return Returns 0 if successful
        [[nodiscard]] int setPassword(const std::string &pwd) const;

        /// Sets libarchive options for the current archive
        /// \param opt The options string
        /// \return Returns 0 if successful
        [[nodiscard]] int setOptions(const std::string &opt) const;

        /// Executes the extraction operation
        /// \param callback The extraction callback function
        /// \return Whether the extraction was successful
        bool extract(const ExtractCallback &callback);

        /// Retrieves the current error code
        /// \return The error code
        [[nodiscard]] int getError()const;

        /// Retrieves the current error message
        /// \return The error message
        [[nodiscard]] const char *getErrorString()const;

        static int openCallback(void *a, ArchiveReader *archive_this);

        static int64_t readCallback(void *a, ArchiveReader *archive_this, const void **buffer);

        static int closeCallback(void *a, ArchiveReader *archive_this);

    protected:
        char buffer[4096]{};
        void *archive{};
        io::InputStream *input{};
    };
}