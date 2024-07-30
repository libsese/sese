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

#include "sese/io/File.h"
#include "ArchiveWriter.h"

#include <archive.h>
#include <archive_entry.h>

#define XX static_cast<struct archive *>(ArchiveWriter::archive)

using sese::archive::ArchiveWriter;

// GCOVR_EXCL_START

SESE_ALWAYS_INLINE int open(struct archive *a, void *archive) {
    return ArchiveWriter::openCallback(a, static_cast<ArchiveWriter *>(archive));
}

#ifdef __APPLE__
SESE_ALWAYS_INLINE long write(archive *a, void *archive, const void *buffer, size_t len) {
#else
SESE_ALWAYS_INLINE int64_t write(struct archive *a, void *archive, const void *buffer, size_t len) {
#endif
    return ArchiveWriter::writeCallback(a, static_cast<ArchiveWriter *>(archive), buffer, len);
}

SESE_ALWAYS_INLINE int close(struct archive *a, void *archive) {
    return ArchiveWriter::closeCallback(a, static_cast<ArchiveWriter *>(archive));
}

SESE_ALWAYS_INLINE int free(struct archive *a, void *archive) {
    return ArchiveWriter::freeCallback(a, static_cast<ArchiveWriter *>(archive));
}

SESE_ALWAYS_INLINE const char *passphrase(struct archive *a,void *archive) {
    return ArchiveWriter::passphraseCallback(a, static_cast<ArchiveWriter *>(archive));
}

// GCOVR_EXCL_STOP

ArchiveWriter::ArchiveWriter(io::OutputStream *output)
        : output(output), archive(archive_write_new()) {
    archive_write_set_passphrase_callback(XX, this, passphrase);
}

ArchiveWriter::~ArchiveWriter() {
    archive_write_free(XX);
}

// GCOVR_EXCL_START

int ArchiveWriter::setFilterGZip() {
    return archive_write_add_filter_gzip(XX);
}

int ArchiveWriter::setFilterBZip2() {
    return archive_write_add_filter_bzip2(XX);
}

int ArchiveWriter::setFilterLZ4() {
    return archive_write_add_filter_lz4(XX);
}

int ArchiveWriter::setFilterLZip() {
    return archive_write_add_filter_lzip(XX);
}

int ArchiveWriter::setFilterZstd() {
    return archive_write_add_filter_zstd(XX);
}

int ArchiveWriter::setFilterXZ() {
    return archive_write_add_filter_xz(XX);
}

int ArchiveWriter::setFilterLzma() {
    return archive_write_add_filter_lzma(XX);
}

int ArchiveWriter::setFilterLzop() {
    return archive_write_add_filter_lzop(XX);
}

int ArchiveWriter::setFilterGRZip() {
    return archive_write_add_filter_grzip(XX);
}

int ArchiveWriter::setFilterLRZip() {
    return archive_write_add_filter_lrzip(XX);
}

int ArchiveWriter::setFilterNone() {
    return archive_write_add_filter_none(XX);
}

int ArchiveWriter::setFormatXar() {
    return archive_write_set_format_xar(XX);
}

int ArchiveWriter::setFormatTar() {
    return archive_write_set_format_ustar(XX);
}

int ArchiveWriter::setFormatZip() {
    return archive_write_set_format_zip(XX);
}

int ArchiveWriter::setFormat7z() {
    return archive_write_set_format_7zip(XX);
}

int ArchiveWriter::setFormatISO() {
    return archive_write_set_format_iso9660(XX);
}

int ArchiveWriter::setPassword(const std::string &pwd) {
    return archive_write_set_passphrase(XX, pwd.c_str());
}

int ArchiveWriter::setOptions(const std::string &opt) {
    return archive_write_set_options(XX, opt.c_str());
}

int ArchiveWriter::getError() {
    return archive_errno(XX);
}

const char *ArchiveWriter::getErrorString() {
    return archive_error_string(XX);
}

// GCOVR_EXCL_STOP

bool ArchiveWriter::begin() {
    if (nullptr == output) {
        return false;
    }

    return ARCHIVE_OK == archive_write_open2(
            XX,
            this,
            open,
            write,
            close,
            free
    );
}

bool ArchiveWriter::done() {
    return ARCHIVE_OK == archive_write_close(XX);
}

bool ArchiveWriter::addPath(const std::filesystem::path &path) {
    return addPath("", path);
}

bool ArchiveWriter::addPath(const std::filesystem::path &base, const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
        return false;
    }

    if (std::filesystem::is_directory(path)) {
        return addDirectory(base, path);
    } else {
        return addFile(base, path);
    }
}

bool ArchiveWriter::addFile(const std::filesystem::path &file) {
    return addFile("", file);
}

bool ArchiveWriter::addFile(const std::filesystem::path &base, const std::filesystem::path &file) {
    auto &&pathname = (base / file.filename()).string();

    struct archive_entry *entry = archive_entry_new();
    archive_entry_set_pathname(entry, pathname.c_str());
    archive_entry_set_size(entry, static_cast<int64_t>(std::filesystem::file_size(file)));
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_write_header(XX, entry);

    int64_t len;
    auto stream = io::File::create(file.string(), io::File::B_READ);
    char buffer[4096];
    while ((len = stream->read(buffer, 4096)) > 0) {
        if (archive_write_data(XX, buffer, static_cast<size_t>(len)) != len) {
            stream->close();
            stream.reset();
            archive_entry_free(entry);
            return false;
        }
    }
    stream->close();
    stream.reset();
    archive_entry_free(entry);
    return true;
}

bool ArchiveWriter::addDirectory(const std::filesystem::path &dir) {
    return addDirectory("", dir);
}

bool ArchiveWriter::addDirectory(const std::filesystem::path &base, const std::filesystem::path &dir) {
    for (const auto &item: std::filesystem::directory_iterator(dir)) {
        if (item.is_directory()) {
            if (ArchiveWriter::addDirectory(base / item.path().filename(), item)) {
                continue;
            } else {
                return false;
            }
        }

        // 压缩包中的相对路径
        auto &&pathname = (base / item.path().filename()).string();
        // 原始文件系统中的绝对路径
        auto &&file_path = item.path().string();

        struct archive_entry *entry = archive_entry_new();
        archive_entry_set_pathname(entry, pathname.c_str());
        archive_entry_set_size(entry, static_cast<int64_t>(std::filesystem::file_size(file_path)));
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_write_header(XX, entry);

        int64_t len;
        auto file = io::File::create(file_path, io::File::B_READ);
        char buffer[4096];
        while ((len = file->read(buffer, 4096)) > 0) {
            if (archive_write_data(XX, buffer, static_cast<size_t>(len)) != len) {
                file->close();
                file.reset();
                archive_entry_free(entry);
                return false;
            }
        }
        file->close();
        file.reset();
        archive_entry_free(entry);
    }
    return true;
}

bool ArchiveWriter::addStream(const std::filesystem::path &path, io::InputStream *input, size_t exp) {
    if (nullptr == input) {
        return false;
    }

    auto &&pathname = path.string();

    struct archive_entry *entry = archive_entry_new();
    archive_entry_set_pathname(entry, pathname.c_str());
    archive_entry_set_size(entry, static_cast<int64_t>(exp));
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_is_data_encrypted(entry, 1);
    archive_write_header(XX, entry);

    while (exp) {
        char buffer[4096];
        const auto LEN = input->read(buffer, std::min<size_t>(exp, 4096));
        if (LEN <= 0) {
            break;
        }
        if (const auto WRITE = archive_write_data(XX, buffer, static_cast<size_t>(LEN));WRITE != LEN) {
            break;
        } else {
            exp -= static_cast<size_t>(LEN);
        }
    }
    archive_entry_free(entry);
    if (exp) {
        return false;
    } else {
        return true;
    }
}

int ArchiveWriter::openCallback(void *archive, ArchiveWriter *archive_this) {
    return ARCHIVE_OK;
}

int64_t ArchiveWriter::writeCallback(void *archive, ArchiveWriter *archive_this, const void *buffer, size_t len) {
    return archive_this->output->write(buffer, len);
}

int ArchiveWriter::closeCallback(void *archive, ArchiveWriter *archive_this) {
    return ARCHIVE_OK;
}

int ArchiveWriter::freeCallback(void *archive, ArchiveWriter *archive_this) {
    return ARCHIVE_OK;
}

const char *ArchiveWriter::passphraseCallback(void *archive, archive::ArchiveWriter *archive_this) {
    return nullptr;
}