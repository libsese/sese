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

#include <sese/archive/ArchiveReader.h>
#include <sese/archive/ArchiveInputStream.h>
#include <sese/io/File.h>

#include <archive.h>
#include <archive_entry.h>

using sese::io::File;
using sese::io::InputStream;
using sese::archive::ArchiveReader;

bool ArchiveReader::extract(const std::filesystem::path &src_path, const std::filesystem::path &dest_path,
                            const std::string &pwd) {
    auto src = src_path.string();
    auto dest = dest_path.string();

    struct archive *a = archive_read_new();
    archive_read_support_compression_all(a);
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    if (!pwd.empty()) {
        if (ARCHIVE_OK != archive_read_add_passphrase(a, pwd.c_str())) {
            archive_read_free(a);
            return false;
        }
    }

    if (ARCHIVE_OK != archive_read_open_filename(a, src.c_str(), 4096)) {
        archive_read_free(a);
        return false;
    }

    struct archive_entry *entry{};
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        std::string filename = archive_entry_pathname(entry);

        auto exp = archive_entry_size(entry);
        auto type = archive_entry_filetype(entry);
        if (AE_IFDIR == type) {
            if ('/' == filename.at(filename.length() - 1)) {
                filename = filename.substr(0, filename.length() - 1);
            }
            auto path = dest_path / filename;
            if (std::filesystem::exists(path)) {
                if (!std::filesystem::is_directory(path)) {
                    archive_read_free(a);
                    return false;
                }
            } else {
                if (!std::filesystem::create_directories(path)) {
                    archive_read_free(a);
                    return false;
                }
            }
            continue;
        }

        auto parent = (dest_path / filename).parent_path();
        if (!std::filesystem::exists(parent)) {
            if (!std::filesystem::create_directories(parent)) {
                archive_read_free(a);
                return false;
            }
        }

        char buffer[4096];
        auto filepath = (dest_path / filename).string();
        auto file = File::create(filepath, BINARY_WRITE_CREATE_TRUNC);
        while (exp) {
            auto len = archive_read_data(a, buffer, std::min<size_t>(exp, 4096));
            if (len <= 0) {
                break;
            }
            auto write = file->write(buffer, static_cast<size_t>(len));
            if (write != len) {
                break;
            } else {
                exp -= len;
            }
        }

        file->flush();
        file->close();
        if (exp) {
            archive_read_free(a);
            return false;
        }
    }

    archive_read_free(a);
    return true;
}

inline int open(struct archive *a, void *data) {
    return ArchiveReader::openCallback(a, static_cast<ArchiveReader *>(data));
}

#ifdef __APPLE__
inline long read(archive *a, void *data, const void **buffer) {
#else
inline int64_t read(struct archive *a, void *data, const void **buffer) {
#endif
    return ArchiveReader::readCallback(a, static_cast<ArchiveReader *>(data), buffer);
}

inline int close(struct archive *a, void *data) {
    return ArchiveReader::closeCallback(a, static_cast<ArchiveReader *>(data));
}

#define XX ((struct archive *) (this->archive))

ArchiveReader::ArchiveReader(io::InputStream *input)
        : input(input), archive(archive_read_new()) {
}

ArchiveReader::~ArchiveReader() {
    archive_read_free(XX);
}

int ArchiveReader::setPassword(const std::string &pwd) {
    return archive_read_add_passphrase(XX, pwd.c_str());
}

int ArchiveReader::setOptions(const std::string &opt) {
    return archive_read_set_options(XX, opt.c_str());
}

bool ArchiveReader::extract(const ArchiveReader::ExtractCallback &callback) {
    archive_read_support_compression_all(XX);
    archive_read_support_filter_all(XX);
    archive_read_support_format_all(XX);
    if (ARCHIVE_OK != archive_read_open(
            XX,
            this,
            open,
            read,
            close
    )) {
        return false;
    }

    struct archive_entry *entry{};
    auto archiveInputStream = ArchiveInputStream(archive);
    while (archive_read_next_header(XX, &entry) == ARCHIVE_OK) {
        std::string filename = archive_entry_pathname(entry);

        auto size = archive_entry_size(entry);
        auto type = archive_entry_filetype(entry);

        callback(filename, Config::fromValue(type), &archiveInputStream, size);
    }
    return true;
}

int ArchiveReader::getError() {
    return archive_errno(XX);
}

const char *ArchiveReader::getErrorString() {
    return archive_error_string(XX);
}

int ArchiveReader::openCallback(void *a, ArchiveReader *_this) {
    return ARCHIVE_OK;
}

int64_t ArchiveReader::readCallback(void *a, ArchiveReader *_this, const void **buffer) {
    auto read = _this->input->read(_this->buffer, 4096);
    *buffer = _this->buffer;
    return read;
}

int ArchiveReader::closeCallback(void *a, ArchiveReader *_this) {
    return ARCHIVE_OK;
}

