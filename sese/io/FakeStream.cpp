#include <sese/io/FakeStream.h>

sese::io::StdInputStreamWrapper::StdInputStreamWrapper(std::istream &stream) : stream(stream) {}

int64_t sese::io::StdInputStreamWrapper::read(void *buffer, size_t length) {
    stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length));
    return stream.gcount();
}

int64_t sese::io::StdInputStreamWrapper::peek(void *buffer, size_t length) {
    stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length));
    const auto L = stream.gcount();
    stream.seekg(-L, std::ios_base::cur);
    return L;
}

int64_t sese::io::StdInputStreamWrapper::trunc(size_t length) {
    stream.seekg(static_cast<std::streamsize>(length), std::ios_base::cur);
    return stream.gcount();
}

sese::io::StdOutputStreamWrapper::StdOutputStreamWrapper(std::ostream &stream) : stream(stream), latest(stream.tellp()) {
}

int64_t sese::io::StdOutputStreamWrapper::write(const void *buffer, size_t length) {
    stream.write(static_cast<const char *>(buffer), static_cast<std::streamsize>(length));
    if (latest == -1) {
        // latest 为 -1 则表示流类型不支持获取写入大小，只能假设写入完成
        return static_cast<int64_t>(length);
    }
    auto now = stream.tellp();
    auto result = now - latest;
    latest = now;
    return result;
}
