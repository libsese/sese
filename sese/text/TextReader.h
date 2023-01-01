#pragma once
#include <sese/Config.h>
#include "sese/util/FileStream.h"
#include "sese/util/BufferedStream.h"
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