#pragma once
#include "Stream.h"
#include <memory>

namespace sese {

    class API AbstractByteBuffer : public Stream {
    public:
        struct Node {
            void *buffer = nullptr;
            Node *next = nullptr;
            size_t length = 0;
            size_t cap = 0;

            explicit Node(size_t bufferSize);
            ~Node();
        };

    public:
        explicit AbstractByteBuffer(size_t baseSize = STREAM_BYTESTREAM_BASE_SIZE);
        ~AbstractByteBuffer();
        void resetPos();
        [[nodiscard]] size_t getLength();
        [[nodiscard]] size_t getCapacity() const;
        /**
         * 释放 CurrentReadNode 前的所有节点
         * @return 实际释放空间，单位 “字节”
         */
        size_t freeCapacity();

    public:
        int64_t read(void *buffer, size_t len) override;
        int64_t write(void *buffer, size_t needWrite) override;
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