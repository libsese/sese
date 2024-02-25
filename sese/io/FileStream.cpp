#include "sese/io/FileStream.h"
#include <cstdio>

#if defined(SESE_PLATFORM_WINDOWS)
#define fseek _fseeki64
#define ftell _ftelli64
#define fileno _fileno
#elif defined(SESE_PLATFORM_LINUX)
#define ftell ftello64
#elif defined(SESE_PLATFORM_APPLE)
#define ftell ftello
#endif

using sese::io::FileStream;

//sese::FileStream::FileStream(const std::string &fileName, const char *mode) {
//#ifdef _WIN32
//    auto rt = fopen_s(&file, fileName.c_str(), mode);
//#else
//    file = fopen(fileName.c_str(), mode);
//    auto rt = errno;
//#endif
//    if (rt != 0) {
//        file = nullptr;
//    }
//}

int64_t FileStream::read(void *buffer, size_t length) {
    return (int64_t)::fread(buffer, 1, length, file);
}

int64_t FileStream::write(const void *buffer, size_t length) {
    return (int64_t)::fwrite(buffer, 1, length, file);
}

void FileStream::close() {
    ::fclose(file);
    file = nullptr;
}

int64_t FileStream::getSeek() {
    return ftell(file);
}

int32_t FileStream::setSeek(int64_t offset, int32_t whence) {
    return fseek(file, offset, whence);
}

int32_t FileStream::flush() {
    return fflush(file);
}

FileStream::Ptr FileStream::create(const std::string &file_path, const char *mode) noexcept {
#ifdef _WIN32
    FILE *file = nullptr;
    if (fopen_s(&file, file_path.c_str(), mode) != 0) {
        return nullptr;
    } else {
        auto stream = new FileStream;
        stream->file = file;
        return std::shared_ptr<FileStream>(stream);
    }
#else
    FILE *file = nullptr;
    file = fopen(filePath.c_str(), mode);
    if (file) {
        auto stream = new FileStream();
        stream->file = file;
        return std::shared_ptr<FileStream>(stream);
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
    this->setSeek(-len, SEEK_CUR);
    return len;
}

int64_t FileStream::trunc(size_t length) {
    auto old_pos = ftell(file);
    this->setSeek((int) length, SEEK_CUR);
    return ftell(file) - old_pos;
}

int32_t FileStream::getFd() const {
    return fileno(file);
}
