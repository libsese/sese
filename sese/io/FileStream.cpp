#include "sese/io/FileStream.h"

#if defined(SESE_PLATFORM_WINDOWS)
#define fseek _fseeki64
#define ftell _ftelli64
#define fileno _fileno
#elif defined(SESE_PLATFORM_LINUX)
#if defined(__GLIBC__)
#define ftell ftello64
#elif defined(__MUSL__)
#define ftell ftello
#endif
#elif defined(SESE_PLATFORM_APPLE)
#define ftell ftello
#endif

using sese::io::FileStream;

int64_t FileStream::read(void *buffer, size_t length) {
    return static_cast<int64_t>(::fread(buffer, 1, length, file)); // GCOVR_EXCL_LINE
}

int64_t FileStream::write(const void *buffer, size_t length) {
    return static_cast<int64_t>(::fwrite(buffer, 1, length, file));
}

void FileStream::close() {
    ::fclose(file);
    file = nullptr;
}

bool FileStream::eof() const {
    return 0 != ::feof(file);
}

int64_t FileStream::getSeek() const {
    return ftell(file);
}

int32_t FileStream::setSeek(int64_t offset, int32_t whence) const {
    return fseek(file, offset, whence);
}

int32_t FileStream::setSeek(int64_t offset, Seek type) const {
    return fseek(file, offset, static_cast<int>(type));
}

int32_t FileStream::flush() const {
    return fflush(file);
}

FileStream::Ptr FileStream::create(const std::string &file_path, const char *mode) noexcept {
#ifdef _WIN32
    FILE *file = nullptr;
    if (fopen_s(&file, file_path.c_str(), mode) != 0) {
        return nullptr;
    } else {
        auto stream = MAKE_SHARED_PRIVATE(FileStream);
        stream->file = file;
        return stream;
    }
#else
    FILE *file = nullptr;
    file = fopen(file_path.c_str(), mode);
    if (file) {
        auto stream = MAKE_SHARED_PRIVATE(FileStream);
        stream->file = file;
        return stream;
    } else {
        return nullptr;
    }
#endif
}

FileStream::Ptr FileStream::createWithPath(const system::Path &path, const char *mode) noexcept {
    return FileStream::create(path.getNativePath(), mode);
}

int64_t FileStream::peek(void *buffer, size_t length) {
    auto len = this->read(buffer, length);
    // GCOVR_EXCL_START
    if (auto rt = this->setSeek(-len, Seek::CUR)) {
        return rt;
    }
    // GCOVR_EXCL_STOP
    return len;
}

int64_t FileStream::trunc(size_t length) {
    auto old_pos = ftell(file);
    // GCOVR_EXCL_START
    if (auto rt = this->setSeek(static_cast<int64_t>(length), Seek::CUR)) {
        return rt;
    }
    // GCOVR_EXCL_STOP
    return ftell(file) - old_pos;
}

int32_t FileStream::getFd() const {
    return fileno(file);
}
