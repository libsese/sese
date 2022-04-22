/**
 * @file AbstractByteBuffer.h
 * @brief 字节缓冲区类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Stream.h"

namespace sese {

    /**
     * @brief 字节缓冲区类
     */
    class API AbstractByteBuffer : public Stream {
    private:
        /**
         * @brief 缓冲节点
         */
        struct Node {
            /// 缓存内存
            void *buffer = nullptr;
            /// 下一个节点
            Node *next = nullptr;
            /// 节点已用内存
            size_t length = 0;
            /// 节点容量
            size_t cap = 0;
            /**
             * 初始化节点
             * @param bufferSize 节点分配内存大小
             */
            explicit Node(size_t bufferSize);
            /// 析构
            ~Node();
        };

    public:
        /**
         * @param baseSize 初始节点内存大小
         */
        explicit AbstractByteBuffer(size_t baseSize = STREAM_BYTE_STREAM_SIZE_FACTOR);

        /// 阻止深拷贝
        AbstractByteBuffer(AbstractByteBuffer &abstractByteBuffer) = delete;
        /// 移动语义
        AbstractByteBuffer(AbstractByteBuffer &&abstractByteBuffer) noexcept;

        /// 析构
        ~AbstractByteBuffer();
        /// 重置读取位置
        virtual void resetPos();
        /**
         * @return 所有节点已用内存总数
         */
        [[nodiscard]] virtual size_t getLength() const;
        /**
         *
         * @return 所有节点容量总数
         */
        [[nodiscard]] virtual size_t getCapacity() const;
        /**
         * 释放 CurrentReadNode 前的所有节点
         * @return 实际释放空间，单位 “字节”
         */
        virtual size_t freeCapacity();

        [[nodiscard]] size_t getCurrentWritePos() const { return currentWritePos; }
        [[nodiscard]] size_t getCurrentReadPos() const { return currentReadPos; }

    public:
        int64_t read(void *buffer, size_t len) override;
        int64_t write(const void *buffer, size_t needWrite) override;
        /**
         * @deprecated close 方法在此处是无用的
         */
        void close() override {}

    private:
        Node *root = nullptr;
        Node *currentWriteNode = nullptr;
        size_t currentWritePos = 0;
        Node *currentReadNode = nullptr;
        size_t currentReadPos = 0;

        /// length 不计算最后一个 Node 的，真实长度应为
        /// realLength = length + currentWriteNode->length
        size_t length = 0;
        size_t cap = 0;
    };

}// namespace sese