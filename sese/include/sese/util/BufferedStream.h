/**
 * @file BufferedStream.h
 * @author kaoru
 * @date 2022年5月24日
 * @brief 流缓存类
 */
#pragma once
#include "Stream.h"
#include "Noncopyable.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /// @brief 流缓存类
    /// @deprecated 请考虑使用 BufferedOutputStream 或 BufferedInputStream
    class API BufferedStream final : public Noncopyable, public Stream {
    public:
        using Ptr = std::shared_ptr<BufferedStream>;

        /**
         * 包装指定流
         * @param source 欲包装的流
         * @param bufferSize 缓存大小
         */
        explicit BufferedStream(const Stream::Ptr &source, size_t bufferSize = STREAM_BYTE_STREAM_SIZE_FACTOR);
        ~BufferedStream() noexcept;

        /**
         * @verbatim
         * 读取一定字节数
         * 若读取大小不超过 BufferedStream 缓存大小，读取将经过自动缓存
         * 反之则直接进行裸流读取，减少拷贝操作
         * @endverbatim
         * @param buffer 用户缓存指针
         * @param length 用户缓存大小
         * @return 实际读取到的字节数
         */
        int64_t read(void *buffer, size_t length) override;

        /**
         * @brief 清空当前读取标志位
         * @warning 执行此操作前请确保 BufferedStream 中的内容已全部读出，否则可能导致必要信息丢失
         */
        void clear() noexcept;

        /**
         * @verbatim
         * 写入一定字节数
         * 若写入大小不超过 BufferedStream 缓存大小，写入将经过自动刷新
         * 反之则直接进行裸流写入，减少拷贝操作
         * @endverbatim
         * @param buffer 用户缓存指针
         * @param length 用户缓存大小
         * @return 实际写入的字节数
         */
        int64_t write(const void *buffer, size_t length) override;

        /**
         * @brief 将当前缓存曲内容全部刷新至流中，并且重置当前写入标志位
         * @return 刷新写入流中的字节数
         */
        int64_t flush() noexcept;

        /// 重置输出源，这会导致原有数据被清除
        /// \param source
        void reset(const Stream::Ptr &source) noexcept;

        [[nodiscard]] size_t getPos() const { return pos; }
        [[nodiscard]] size_t getLen() const { return len; }
        [[nodiscard]] size_t getCap() const { return cap; }

    private:
        inline int64_t preRead();

        /// 源流指针
        Stream::Ptr source;
        /// 缓存指针
        void *buffer = nullptr;
        /// 已操作的数据大小
        size_t pos;
        /// 可供操作的数据大小
        size_t len;
        /// 缓存容量
        size_t cap;
    };
}// namespace sese