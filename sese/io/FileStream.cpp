#include "sese/io/FileStream.h"
#include <cstdio>

#if defined(SESE_PLATFORM_WINDOWS)
#define fseek _fseeki64
#define ftell _ftelli64
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

//bool sese::FileStream::open(const std::string &fileName, const char *mode) noexcept {
//#ifdef _WIN32
//    auto rt = fopen_s(&file, fileName.c_str(), mode);
//    if (rt != 0) {
//        file = nullptr;
//        return false;
//    } else {
//        return true;
//    }
//#else
//    file = fopen(fileName.c_str(), mode);
//    if (file) {
//        return true;
//    } else {
//        return false;
//    }
//#endif
//}

FileStream::Ptr FileStream::create(const std::string &fileName, const char *mode) noexcept {
#ifdef _WIN32
    FILE *file = nullptr;
    if (fopen_s(&file, fileName.c_str(), mode) != 0) {
        return nullptr;
    } else {
        auto stream = new FileStream;
        stream->file = file;
        return std::shared_ptr<FileStream>(stream);
    }
#else
    FILE *file = nullptr;
    file = fopen(fileName.c_str(), mode);
    if (file) {
        auto stream = new FileStream();
        stream->file = file;
        return std::shared_ptr<FileStream>(stream);
    } else {
        return nullptr;
    }
#endif
}

int64_t FileStream::peek(void *buffer, size_t length) {
    auto len = this->read(buffer, length);
    this->setSeek(-len, SEEK_CUR);
    return len;
}

int64_t FileStream::trunc(size_t length) {
    auto oldPos = ftell(file);
    this->setSeek((int) length, SEEK_CUR);
    return ftell(file) - oldPos;
}
