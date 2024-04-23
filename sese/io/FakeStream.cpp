#include <sese/io/FakeStream.h>

sese::io::StdInputStreamWrapper::StdInputStreamWrapper(std::istream &stream) : stream(stream) {}

int64_t sese::io::StdInputStreamWrapper::read(void *buffer, size_t length) override {
    return stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length)).get();
}

int64_t sese::io::StdInputStreamWrapper::peek(void *buffer, size_t length) {
    auto l = stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length)).get();
    stream.seekg(-l, std::ios_base::cur);
    return l;
}

int64_t sese::io::StdInputStreamWrapper::trunc(size_t length) {
    return stream.seekg(static_cast<std::streamsize>(length), std::ios_base::cur).get();
}

sese::io::StdOutputStreamWrapper::StdOutputStreamWrapper(std::ostream &stream) : stream(stream), latest(stream.tellp()) {
}

int64_t sese::io::StdOutputStreamWrapper::write(const void *buffer, size_t length) override {
    stream.write(static_cast<const char *>(buffer), static_cast<std::streamsize>(length));
    auto now = stream.tellp();
    auto result = now - latest;
    latest = now;
    return result;
}
