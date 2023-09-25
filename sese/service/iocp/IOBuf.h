/**
 * @file IOBuf.h
 * @brief 用于 IOCP 的链式缓存
 * @author kaoru
 * @date 2023年9月20日
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/InputStream.h>
#include <sese/io/PeekableStream.h>

namespace sese::iocp {

    /// IOBuf 节点
    struct IOBufNode {
        /// 缓存指针
        void *buffer{nullptr};
        /// 缓存已读大小
        size_t read{0};
        /// 缓存已填充大小
        size_t size{0};
        /// 缓存容量大小
        const size_t capacity;
        /// 下一个节点
        IOBufNode *next{nullptr};

        /**
         * 新建一个节点
         * @param capacity 节点容量大小
         */
        explicit IOBufNode(size_t capacity);

        /// 节点析构
        ~IOBufNode();

        /**
         * 获取节点可读部分大小
         * @return 可读大小
         */
        [[nodiscard]] size_t getReadableSize() const noexcept;

        /**
         * 获取节点可写入大小
         * @return 可写大小（空闲部分）
         */
        [[nodiscard]] size_t getWriteableSize() const noexcept;
    };

    /// 用于 IOCP 的链式缓存
    class IOBuf : public io::InputStream, public io::PeekableStream {
    public:
        /// 节点类型
        using Node = IOBufNode;

        /// 析构将释放所有节点
        ~IOBuf() override;

        /// 添加一个新的节点，节点一旦被添加，外部就不可继续变更此节点
        /// \param node 目标节点
        void push(Node *node);

        /// 释放所有节点
        void clear();

        /// 获取当前可读大小
        [[nodiscard]] size_t getReadableSize() const noexcept;

        /// 获取当前所有节点内容大小
        [[nodiscard]] size_t getTotalSize() const noexcept;

        /// 从一个或多个节点读取内容
        /// \param buffer 缓存
        /// \param length 缓存大小
        /// \return 实际获取大小
        int64_t read(void *buffer, size_t length) override;

        /// 从一个或多个节点窥视内容
        /// \param buffer 缓存
        /// \param length 缓存大小
        /// \return 实际获取大小
        int64_t peek(void *buffer, size_t length) override;

        /// 从一个或多个节点步进内容
        /// \param length 步进大小
        /// \return 实际步进大小
        int64_t trunc(size_t length) override;

    private:
        Node *root = nullptr;
        Node *tail = nullptr;
        Node *cur = nullptr;

        size_t total{0};
        size_t readed{0};
    };

}// namespace sese::iocp
