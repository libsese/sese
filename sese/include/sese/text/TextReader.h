#pragma once
#include <sese/Config.h>
#include "sese/io/FileStream.h"
#include "sese/io/BufferedStream.h"
#include <sese/text/String.h>

namespace sese::text {

    class API TextReader final {
    public:
        using Ptr = std::shared_ptr<TextReader>;

        ~TextReader() noexcept;
        static std::shared_ptr<sese::text::TextReader> create(const char *u8str) noexcept;
        String readLine();

    private:
        TextReader() = default;

        io::FileStream::Ptr fileStream;
        io::BufferedStream::Ptr bufferedStream;
    };
}