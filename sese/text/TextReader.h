/**
* @file TextReader.h
* @author kaoru
* @version 0.1
* @brief 文本读取类
* @date 2023年9月13日
*/

#pragma once

#include <sese/Config.h>
#include <sese/io/FileStream.h>
#include <sese/io/BufferedStream.h>
#include <sese/text/String.h>

namespace sese::text {

/// 文本读取类
class  TextReader final {
public:
    /// 文本读取类智能指针
    using Ptr = std::shared_ptr<TextReader>;

    ~TextReader() noexcept;
    static std::shared_ptr<sese::text::TextReader> create(const char *u8str) noexcept;
    String readLine();

private:
    TextReader() = default;

    io::FileStream::Ptr fileStream;
    io::BufferedStream::Ptr bufferedStream;
};
} // namespace sese::text