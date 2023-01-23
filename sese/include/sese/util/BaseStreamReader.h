/**
 * @file BaseStreamReader.h
 * @author kaoru
 * @brief 输出流包装类
 * @date 2022年4月13日
 */
#pragma once
#include "ByteBuilder.h"
#include "Stream.h"
#include "Util.h"
#include <sstream>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {
    /**
     * @brief 输出流包装类，Windows 下 UTF-8 需要特殊处理
     */
    template<typename T = char>
    class API BaseStreamReader {
    public:
        using Ptr = std::shared_ptr<BaseStreamReader<T>>;

    public:
        /**
         * @brief 构造函数
         * @param source 欲包装流
         */
        explicit BaseStreamReader(const Stream::Ptr &source) {
            this->sourceStream = source;
            this->bufferStream = std::make_unique<ByteBuilder>();
        }

        /**
         * @brief 读取一个字符
         * @param ch 字符容器
         * @return 读取是否成功
         */
        bool read(T &ch) {
            if (bufferStream->read(&ch, sizeof(T)) == 0) {
                /// 未命中缓存，尝试从源中读取
                auto len = preRead();
                if (0 == len) {
                    /// FileStream 读至文件尾返回值为 0
                    /// 源已被读完
                    return false;
                }
            }
            /// 嘿嘿，读取到一个字符
            return true;
        }

        /**
         * @brief 读取一行
         * @return 返回读取的字符串，长度为零测读取完成
         */
        std::basic_string<T> readLine() {
            std::basic_stringstream<T> string;
            auto canReadSize = bufferStream->getLength() - bufferStream->getCurrentReadPos();
            if (canReadSize == 0) {
                canReadSize += preRead();
            }

            if (canReadSize > 0) {
                /// 有东西可读
                T ch;
                bool isFirst = true;
                while (read(ch)) {
                    if (ch == '\r' || ch == '\n') {
                        if (isFirst) continue;
                        break;
                    }
                    string << ch;
                    isFirst = false;
                }
            }
            bufferStream->freeCapacity();
            return string.str();
        }

        /**
         *  @brief 由于 BaseStreamReader 的缓存机制，导致缓存尾部数据可能丢失，该函数返回超前未被读取部分数据长度
         * @return 超前数据长度
         */
        [[nodiscard]] size_t getAheadLength() const { return bufferStream->getLength() - bufferStream->getCurrentReadPos(); }

        /**
         * 用于缓存超前部分数据
         * @return 缓存
         */
        const ByteBuilder::Ptr &getBuffer() noexcept { return this->bufferStream; }

    private:
        int64_t preRead() {
            char buffer[STREAM_BYTE_STREAM_SIZE_FACTOR];
            auto len = sourceStream->read(buffer, STRING_BUFFER_SIZE_FACTOR);
            if (0 < len) {
                bufferStream->write(buffer, len);
            }
            return len;
        }

    private:
        Stream::Ptr sourceStream;
        ByteBuilder::Ptr bufferStream;
    };

    using StreamReader = BaseStreamReader<char>;
    using WStreamReader = BaseStreamReader<wchar_t>;
}// namespace sese