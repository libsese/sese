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

/// \file ArchiveWriter.h
/// \brief Archive writer
/// \author Kaoru
/// \date January 9, 2024

#pragma once

#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>

#include <filesystem>

namespace sese::archive {

    /// Archive writer
    class ArchiveWriter {
    public:
        /// Constructs the writer
        /// \param output The output stream
        explicit ArchiveWriter(io::OutputStream *output);

        virtual ~ArchiveWriter();

        int setFilterGZip();

        int setFilterBZip2();

        int setFilterLZ4();

        int setFilterLZip();

        int setFilterZstd();

        int setFilterXZ();

        int setFilterLzma();

        int setFilterLzop();

        int setFilterGRZip();

        int setFilterLRZip();

        int setFilterNone();

        int setFormatXar();

        int setFormatTar();

        int setFormatZip();

        int setFormat7z();

        int setFormatISO();

        /// Sets the password used for writing to the archive
        /// \param pwd The password
        /// \return Returns 0 if successful
        int setPassword(const std::string &pwd);

        /// Sets the libarchive options for the archive
        /// \param opt The options
        /// \return Returns 0 if successful
        int setOptions(const std::string &opt);

        /// Retrieves the current error code
        /// \return The error code
        int getError();

        /// Retrieves the current error message
        /// \return The error message
        const char *getErrorString();

        /// Starts the entity addition
        /// \return The result of the operation
        bool begin();

        /// Ends the entity addition
        /// \return The result of the operation
        bool done();

        /// Entity operation function, adds a file or folder to the root directory of the current archive
        /// \param path The target path
        /// \return Whether the operation was successful
        bool addPath(const std::filesystem::path &path);

        /// Entity operation function, adds a file or folder to the specified path in the current archive
        /// \param base The path within the archive
        /// \param path The target path
        /// \return Whether the operation was successful
        bool addPath(const std::filesystem::path &base, const std::filesystem::path &path);

        bool addFile(const std::filesystem::path &file);

        bool addFile(const std::filesystem::path &base, const std::filesystem::path &file);

        bool addDirectory(const std::filesystem::path &dir);

        bool addDirectory(const std::filesystem::path &base, const std::filesystem::path &dir);

        /// Entity operation function, reads content from the stream and writes it to the specified path in the archive
        /// \param path The path within the archive
        /// \param input The input stream
        /// \param len The size to be written
        /// \return Whether the operation was successful
        bool addStream(const std::filesystem::path &path, io::InputStream *input, size_t len);

        static int openCallback(void *archive, ArchiveWriter *archive_this);

        static int64_t writeCallback(void *archive, ArchiveWriter *archive_this, const void *buffer, size_t len);

        static int closeCallback(void *archive, ArchiveWriter *archive_this);

        static int freeCallback(void *archive, ArchiveWriter *archive_this);

        static const char *passphraseCallback(void *archive, ArchiveWriter *archive_this);

    protected:
        io::OutputStream *output{};
        void *archive{};
    };
}