/**
 * @file StreamReader.h
 * @author kaoru
 * @brief 输出流包装类
 * @date 2022年4月13日
 */
#pragma once
#include "Stream.h"
#include "ByteBuilder.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {
    /**
     * @brief 输出流包装类，Windows 下不支持 UTF-8 文本处理
     */
    class API StreamReader {
    public:
        /**
         * @brief 构造函数
         * @param source 欲包装流
         */
        explicit StreamReader(Stream::Ptr source);

        /**
         * @brief 读取一个字符
         * @param ch 字符容器
         * @return 读取是否成功
         */
        bool read(char &ch);

        /**
         * @brief 读取一行
         * @return 返回读取的字符串，长度为零测读取完成
         */
        std::string readLine();

        /**
         *  @brief 由于 StreamReader 的缓存机制，导致缓存尾部数据可能丢失，该函数返回超前未被读取部分数据长度
         * @return 超前数据长度
         */
        [[nodiscard]] size_t getAheadLength() const;

        /**
         * @return 返回超前数据缓存，配合 freeAheadBuffer 使用
         */
        [[nodiscard]] void *getAheadBuffer() const;

        /**
         * @param buffer 释放缓存
         */
        static void freeAheadBuffer(void *buffer) ;

    private:
        int64_t preRead();

    private:
        Stream::Ptr sourceStream;
        ByteBuilder::Ptr bufferStream;
    };
}// namespace sese