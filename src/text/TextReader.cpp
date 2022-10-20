#include <sese/text/TextReader.h>

namespace sese::text {

    TextReader::~TextReader() noexcept {
        fileStream->close();
    }

    bool TextReader::open(const char *u8str) noexcept {
        fileStream = FileStream::create(u8str, BINARY_READ_EXISTED);
        bufferedStream = std::make_shared<BufferedStream>(fileStream);
        return nullptr != fileStream;
    }

    String TextReader::readLine() {
        size_t offset = 0;
        size_t i = 0;
        char buffer[1024];
        while (i < 1020) {
            auto success = bufferedStream->read(buffer + i - offset, 1);
            if (1 == success) {
                if ('\r' == buffer[i - offset] || '\n' == buffer[i - offset]) {
                    if (0 == i) {
                        offset += 1;
                        continue;
                    } else {
                        buffer[i - offset] = 0;
                        return {buffer, i - offset};
                    }
                }

                auto size = sstr::getSizeFromUTF8Char(*(buffer + i - offset));

                if (size > 1) {
                    bufferedStream->read(buffer + i + 1 - offset, size - 1);
                }
                i += size;
            } else {
                if (0 == i) {
                    return String();
                } else {
                    buffer[i] = 0;
                    return {buffer, i - offset};
                }
            }
        }
        return {buffer, i - offset};
    }
}// namespace sese::text