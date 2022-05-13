/**
 * @file UniReader.h
 * @brief 跨平台的文件读取器，主要适配 Windows，支持 UNICODE
 * @author kaoru
 * @date 2022年4月15日
 */
#pragma once
#include "sese/BaseStreamReader.h"
#include "sese/FileStream.h"

namespace sese {

    /**
     * @brief 跨平台的文件读取器，主要适配 Windows，支持 UNICODE
     */
    class API UniReader {
    public:
        using Ptr = std::shared_ptr<UniReader>;

        UniReader() = default;
        ~UniReader();

        /**
         * 打开文件
         * @param fileName 文件名称
         * @return 打开是否成功
         */
        bool open(const std::string &fileName) noexcept;

        /**
         * 读取一行
         * @return 读取到的文本，当 string.empty() 为 true 时表示没有内容可供再读取
         */
        std::string readLine();

    private:
        FileStream::Ptr fileStream;
#ifdef _WIN32
        WStreamReader::Ptr reader;
#else
        StreamReader::Ptr reader;
#endif
    };
}