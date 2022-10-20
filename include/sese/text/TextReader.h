#pragma once
#include <sese/Config.h>
#include <sese/FileStream.h>
#include <sese/BufferedStream.h>
#include <sese/text/String.h>

namespace sese::text {

    class API TextReader {
    public:
        using Ptr = std::shared_ptr<TextReader>;

        TextReader() = default;
        ~TextReader() noexcept;
        bool open(const char *u8str) noexcept;
        String readLine();

    private:
        FileStream::Ptr fileStream;
        BufferedStream::Ptr bufferedStream;
    };
}