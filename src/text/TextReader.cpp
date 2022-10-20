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
        size_t i = 0;
        char buffer[1024];
        while (i < 1020) {
            auto success = bufferedStream->read(buffer + i, 1);
            if (1 == success) {
                if ('\r' == buffer[i] || '\n' == buffer[i]) {
                    if (0 == i) {
                        continue;
                    } else {
                        buffer[i] = 0;
                        return {buffer, i};
                    }
                }

                auto size = sstr::getSizeFromUTF8Char(*(buffer + i));

                if (size > 1) {
                    bufferedStream->read(buffer + i + 1, size - 1);
                }
                i += size;
            } else {
                if (0 == i) {
                    return String();
                } else {
                    buffer[i] = 0;
                    return {buffer, i};
                }
            }
        }
        return {buffer, i};
    }
}// namespace sese::text