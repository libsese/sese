#include "FileStream.h"

#ifdef _WIN32
#define fseek _fseeki64
#endif

sese::FileStream::FileStream(const std::string &fileName, const char *mode) {
#ifdef _WIN32
    auto rt = fopen_s(&file, fileName.c_str(), mode);
#else
    file = fopen(fileName.c_str(), mode);
    auto rt = errno;
#endif
    if (rt != 0) {
        file = nullptr;
    }
}

int64_t sese::FileStream::read(void *buffer, size_t length) {
    return (int64_t) ::fread(buffer, 1, length, file);
}

int64_t sese::FileStream::write(const void *buffer, size_t length) {
    return (int64_t) ::fwrite(buffer, 1, length, file);
}

void sese::FileStream::close() {
    if(file) {
        ::fclose(file);
        file = nullptr;
    }
}

int64_t sese::FileStream::getSeek() {
    return ::ftell(file);
}

int32_t sese::FileStream::setSeek(int64_t offset, int32_t whence) {
    return ::fseek(file, offset, whence);
}
