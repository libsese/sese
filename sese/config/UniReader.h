/**
 * @file UniReader.h
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UNICODE
 * @author kaoru
 * @date 2022年4月15日
 */

#pragma once

#include "sese/io/BaseStreamReader.h"
#include "sese/io/FileStream.h"

#ifdef SESE_PLATFORM_WINDOWS
#include <codecvt>
#ifdef __MINGW64__
#include <locale>
#endif
#endif

namespace sese {

/**
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UTF-8
 * @deprecated 请考虑使用 sese::text::TextReader
 */
class API UniReader final {
public:
    using Ptr = std::shared_ptr<UniReader>;

    ~UniReader();

    /**
     * 打开文件
     * @param fileName 文件名称
     * @retval nullptr 打开失败
     */
    static std::shared_ptr<UniReader> create(const std::string &fileName) noexcept;

    /**
     * 读取一行
     * @return 读取到的文本，当 string.empty() 为 true 时表示没有内容可供再读取
     */
    std::string readLine();

private:
    UniReader() = default;

    io::FileStream::Ptr fileStream;
#ifdef SESE_PLATFORM_WINDOWS
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    io::WStreamReader::Ptr reader;
#else
    io::StreamReader::Ptr reader;
#endif
};
} // namespace sese