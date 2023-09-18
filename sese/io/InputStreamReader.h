/// \file InputStreamReader.h
/// \author kaoru
/// \brief 可读流读取器
/// \date 2023年9月11日

#pragma once

#include <sese/io/PeekableStream.h>
#include <sese/text/StringBuilder.h>

namespace sese::io {

    /// 可读流读取器
    class API InputStreamReader {
    public:
        explicit InputStreamReader(io::PeekableStream *input) noexcept;

        /// 读取一行
        /// \note 支持 CR、LF、CRLF 换行分隔符
        /// \return 如果内容全部读完（例如文件流的 EOF），则返回 std::string {}
        std::string readLine() noexcept;

    protected:
        PeekableStream *input;
        text::StringBuilder builder;
    };
}