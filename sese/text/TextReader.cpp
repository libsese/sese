#include <sese/text/TextReader.h>

namespace sese::text {

TextReader::~TextReader() noexcept {
    fileStream->close();
}

std::shared_ptr<sese::text::TextReader> TextReader::create(const char *u8str) noexcept {
    auto fileStream = io::FileStream::create(u8str, BINARY_READ_EXISTED);
    if (fileStream == nullptr) {
        return nullptr;
    }

    auto bufferedStream = std::make_shared<io::BufferedStream>(fileStream);
    auto reader = new TextReader;
    reader->fileStream = fileStream;
    reader->bufferedStream = bufferedStream;
    return std::shared_ptr<sese::text::TextReader>(reader);
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
} // namespace sese::text