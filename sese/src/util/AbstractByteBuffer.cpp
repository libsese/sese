#include "sese/util/AbstractByteBuffer.h"

#include <cstring>

namespace sese {

    AbstractByteBuffer::Node::Node(size_t bufferSize) {
        this->buffer = malloc(bufferSize);
        this->cap = bufferSize;
    }

    AbstractByteBuffer::Node::~Node() {
        free(this->buffer);
    }

    AbstractByteBuffer::AbstractByteBuffer(size_t baseSize) {
        this->root = new Node(baseSize);// GCOVR_EXCL_LINE
        this->cap = baseSize;
        this->currentWriteNode = root;
        this->currentReadNode = root;
    }

    AbstractByteBuffer::AbstractByteBuffer(AbstractByteBuffer &src) noexcept {
        // this->root = new Node(abstractByteBuffer.cap);
        // this->cap = abstractByteBuffer.cap;
        // this->length = 0;
        // this->currentWriteNode = root;
        // this->currentWritePos = abstractByteBuffer.length + abstractByteBuffer.currentWritePos;
        // this->currentReadNode = root;
        // this->currentReadPos = abstractByteBuffer.length + abstractByteBuffer.currentReadPos;
        // size_t copyPos = 0;
        // Node *pNode = abstractByteBuffer.root;
        // if (pNode != nullptr) {
        //     while (true) {
        //         if(pNode == abstractByteBuffer.currentReadNode) {
        //             this->currentReadPos = copyPos + abstractByteBuffer.currentReadPos;
        //         }
        //         memcpy((char *) root->buffer + copyPos, pNode->buffer, pNode->length);
        //         if (pNode->next == nullptr) {
        //             break;
        //         } else {
        //             pNode = pNode->next;
        //             copyPos += pNode->length;
        //         }
        //     }
        // }
        /// 根节点属性
        this->root = new Node(src.root->cap);
        this->root->length = src.root->length;
        memcpy(this->root->buffer, src.root->buffer, src.root->length);
        this->currentWriteNode = this->root;
        this->currentReadNode = this->root;

        /// 拷贝子节点
        Node *pLastNode = this->root;
        Node *pNode = src.root->next;
        while (pNode) {
            auto sub = new Node(pNode->cap);
            sub->length = pNode->length;
            memcpy(sub->buffer, pNode->buffer, pNode->length);

            if (pNode == src.currentWriteNode) {// GCOVR_EXCL_LINE
                this->currentWriteNode = sub;
            }
            if (pNode == src.currentReadNode) {// GCOVR_EXCL_LINE
                this->currentReadNode = sub;
            }

            pLastNode->next = sub;
            pLastNode = sub;

            pNode = pNode->next;
        }

        /// 拷贝全局属性
        this->cap = src.cap;
        this->length = src.length;
        this->currentReadPos = src.currentReadPos;
        this->currentWritePos = src.currentWritePos;
    }

    AbstractByteBuffer::AbstractByteBuffer(AbstractByteBuffer &&abstractByteBuffer) noexcept {
        this->root = abstractByteBuffer.root;
        this->currentWriteNode = abstractByteBuffer.currentWriteNode;
        this->currentWritePos = abstractByteBuffer.currentWritePos;
        this->currentReadNode = abstractByteBuffer.currentReadNode;
        this->currentReadPos = abstractByteBuffer.currentReadPos;
        this->length = abstractByteBuffer.length;
        this->cap = abstractByteBuffer.cap;

        abstractByteBuffer.root = nullptr;
        abstractByteBuffer.currentWriteNode = nullptr;
        abstractByteBuffer.currentWritePos = 0;
        abstractByteBuffer.currentReadNode = nullptr;
        abstractByteBuffer.currentReadPos = 0;
        abstractByteBuffer.length = 0;
        abstractByteBuffer.cap = 0;
    }

    AbstractByteBuffer::~AbstractByteBuffer() {
        Node *toDel;
        while (root != nullptr) {
            toDel = root;
            root = toDel->next;
            delete toDel;// GCOVR_EXCL_LINE
        }
    }

    void AbstractByteBuffer::resetPos() {
        this->currentReadNode = root;
        this->currentReadPos = 0;
    }

    size_t AbstractByteBuffer::getLength() const {
        return this->length + currentWriteNode->length;
    }

    size_t AbstractByteBuffer::getCapacity() const {
        return this->cap;
    }

    size_t AbstractByteBuffer::freeCapacity() {
        // size_t freeCap = 0;
        // Node *toDel;
        // while (root != this->currentReadNode && root->next) {
        //     toDel = root;
        //     root = toDel->next;
        //     freeCap += toDel->cap;
        //     delete toDel;// GCOVR_EXCL_LINE
        // }
        // length -= freeCap;
        // cap -= freeCap;
        // return freeCap;
        size_t freeCap = 0;
        while(root != currentReadNode) {
            freeCap += root->cap;
            auto toDel = root;
            root = root->next;
            delete toDel;// GCOVR_EXCL_LINE
        }
        length -= freeCap;
        cap -= freeCap;

        currentReadPos = 0;
        currentWritePos = 0;
        root->length = 0;

        return freeCap;
    }

    int64_t AbstractByteBuffer::read(void *buffer, size_t needRead) {
        int64_t actualRead = 0;
        while (true) {
            // 当前单元能提供的剩余读取量
            size_t currentReadNodeRemaining = currentReadNode->length - currentReadPos;
            // 当前单元能满足读取需求
            if (needRead <= currentReadNodeRemaining) {
                memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, needRead);
                actualRead += (int64_t) needRead;
                currentReadPos += needRead;
                break;
            }
            // 当前单元不能满足读取需求
            else {
                memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, currentReadNodeRemaining);
                actualRead += (int64_t) currentReadNodeRemaining;
                needRead -= currentReadNodeRemaining;
                currentReadPos += currentReadNodeRemaining;
                if (currentReadNode == currentWriteNode) {
                    // 已经没有剩余节点可供读取
                    break;
                } else {
                    // 切换下一个节点继续读取
                    currentReadNode = currentReadNode->next;
                    currentReadPos = 0;
                    continue;
                }
            }
        }
        return actualRead;
    }

    int64_t AbstractByteBuffer::write(const void *buffer, size_t needWrite) {
        int64_t actualWrite = 0;
        while (true) {
            // 当前单元能提供的剩余写入量
            size_t currentWriteNodeRemaining = currentWriteNode->cap - currentWriteNode->length;
            // 当前单元能满足写入需求
            if (needWrite <= currentWriteNodeRemaining) {
                memcpy((char *) currentWriteNode->buffer + currentWritePos, (char *) buffer + actualWrite, needWrite);
                actualWrite += (int64_t) needWrite;
                currentWritePos += needWrite;
                currentWriteNode->length += needWrite;
                break;
            }
            // 当前单元不能满足读取需求
            else {
                memcpy((char *) currentWriteNode->buffer + currentWritePos, (char *) buffer + actualWrite, currentWriteNodeRemaining);
                actualWrite += (int64_t) currentWriteNodeRemaining;
                needWrite -= currentWriteNodeRemaining;
                currentWriteNode->length = currentWriteNode->cap;

                // 更新全局信息
                length += currentWriteNode->cap;
                cap += STREAM_BYTE_STREAM_SIZE_FACTOR;
                // 直接切换至下一个单元
                currentWritePos = 0;
                currentWriteNode->next = new Node(STREAM_BYTE_STREAM_SIZE_FACTOR);// GCOVR_EXCL_LINE
                currentWriteNode = currentWriteNode->next;
                continue;
            }
        }
        return actualWrite;
    }

    int64_t AbstractByteBuffer::peek(void *buffer, size_t needRead) {
        const Node *_currentReadNode = currentReadNode;
        const Node *_currentWriteNode = currentWriteNode;
        auto _currentReadPos = currentReadPos;

        int64_t actualRead = 0;
        while (true) {
            // 当前单元能提供的剩余读取量
            size_t currentReadNodeRemaining = _currentReadNode->length - _currentReadPos;
            // 当前单元能满足读取需求
            if (needRead <= currentReadNodeRemaining) {
                memcpy((char *) buffer + actualRead, (char *) _currentReadNode->buffer + _currentReadPos, needRead);
                actualRead += (int64_t) needRead;
                _currentReadPos += needRead;
                break;
            }
            // 当前单元不能满足读取需求
            else {
                memcpy((char *) buffer + actualRead, (char *) _currentReadNode->buffer + _currentReadPos, currentReadNodeRemaining);
                actualRead += (int64_t) currentReadNodeRemaining;
                needRead -= currentReadNodeRemaining;
                _currentReadPos += currentReadNodeRemaining;
                if (_currentReadNode == _currentWriteNode) {
                    // 已经没有剩余节点可供读取
                    break;
                } else {
                    // 切换下一个节点继续读取
                    _currentReadNode = _currentReadNode->next;
                    _currentReadPos = 0;
                    continue;
                }
            }
        }
        return actualRead;
    }

    int64_t AbstractByteBuffer::trunc(size_t needRead) {
        int64_t actualRead = 0;
        while (true) {
            // 当前单元能提供的剩余读取量
            size_t currentReadNodeRemaining = currentReadNode->length - currentReadPos;
            // 当前单元能满足读取需求
            if (needRead <= currentReadNodeRemaining) {
                // memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, length);
                actualRead += (int64_t) needRead;
                currentReadPos += needRead;
                break;
            }
            // 当前单元不能满足读取需求
            else {
                // memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, currentReadNodeRemaining);
                actualRead += (int64_t) currentReadNodeRemaining;
                needRead -= currentReadNodeRemaining;
                currentReadPos += currentReadNodeRemaining;
                if (currentReadNode == currentWriteNode) {
                    // 已经没有剩余节点可供读取
                    break;
                } else {
                    // 切换下一个节点继续读取
                    currentReadNode = currentReadNode->next;
                    currentReadPos = 0;
                    continue;
                }
            }
        }
        return actualRead;
    }

}// namespace sese