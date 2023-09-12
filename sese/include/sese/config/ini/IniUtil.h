#pragma once

#include <sese/config/ini/IniConfig.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

namespace sese::config::ini {

    class API IniUtil {
    public:
        using InputStream = io::PeekableStream;
        using OutputStream = io::OutputStream;
        using IniConfigObject = std::unique_ptr<IniConfig>;

        static IniConfigObject parse(InputStream *input) noexcept;

        static bool unparse(const IniConfigObject &target, OutputStream *output) noexcept;

    protected:
        static bool unparsePair(const std::pair<std::string ,std::string> &pair, OutputStream *output) noexcept;
    };

}