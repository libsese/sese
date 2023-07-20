/**
 * @file UniReader.h
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UNICODE
 * @author kaoru
 * @date 2022年4月15日
 */
#pragma once
#include "sese/util/BaseStreamReader.h"
#include "sese/util/FileStream.h"

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

        FileStream::Ptr fileStream;
#ifdef _WIN32
        WStreamReader::Ptr reader;
#else
        StreamReader::Ptr reader;
#endif
    };
}