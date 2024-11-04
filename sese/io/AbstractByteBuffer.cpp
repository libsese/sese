// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/io/AbstractByteBuffer.h"

#include <cstring>

namespace sese::io {

AbstractByteBuffer::Node::Node(size_t buffer_size) {
    this->buffer = malloc(buffer_size);
    this->cap = buffer_size;
}

AbstractByteBuffer::Node::~Node() {
    free(this->buffer);
}

AbstractByteBuffer::AbstractByteBuffer(size_t base_size, size_t factor) {
    this->root = new Node(base_size); // GCOVR_EXCL_LINE
    this->cap = base_size;
    this->currentWriteNode = root;
    this->currentReadNode = root;
    this->factor = factor;
}

AbstractByteBuffer::AbstractByteBuffer(const AbstractByteBuffer &src) noexcept {
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
    Node *p_last_node = this->root;
    const Node *p_node = src.root->next;
    while (p_node) {
        const auto SUB = new Node(p_node->cap);
        SUB->length = p_node->length;
        memcpy(SUB->buffer, p_node->buffer, p_node->length);

        if (p_node == src.currentWriteNode) { // GCOVR_EXCL_LINE
            this->currentWriteNode = SUB;
        }
        if (p_node == src.currentReadNode) { // GCOVR_EXCL_LINE
            this->currentReadNode = SUB;
        }

        p_last_node->next = SUB;
        p_last_node = SUB;

        p_node = p_node->next;
    }

    /// 拷贝全局属性
    this->cap = src.cap;
    this->length = src.length;
    this->currentReadPos = src.currentReadPos;
    this->currentWritePos = src.currentWritePos;
    this->factor = src.factor;
}

AbstractByteBuffer::AbstractByteBuffer(AbstractByteBuffer &&abstract_byte_buffer) noexcept {
    this->root = abstract_byte_buffer.root;
    this->currentWriteNode = abstract_byte_buffer.currentWriteNode;
    this->currentWritePos = abstract_byte_buffer.currentWritePos;
    this->currentReadNode = abstract_byte_buffer.currentReadNode;
    this->currentReadPos = abstract_byte_buffer.currentReadPos;
    this->length = abstract_byte_buffer.length;
    this->cap = abstract_byte_buffer.cap;
    this->factor = abstract_byte_buffer.factor;

    abstract_byte_buffer.root = nullptr;
    abstract_byte_buffer.currentWriteNode = nullptr;
    abstract_byte_buffer.currentWritePos = 0;
    abstract_byte_buffer.currentReadNode = nullptr;
    abstract_byte_buffer.currentReadPos = 0;
    abstract_byte_buffer.length = 0;
    abstract_byte_buffer.cap = 0;
    abstract_byte_buffer.factor = 0;
}

AbstractByteBuffer::~AbstractByteBuffer() {
    while (root != nullptr) {
        const Node *to_del = root;
        root = to_del->next;
        delete to_del; // GCOVR_EXCL_LINE
    }
}

void AbstractByteBuffer::resetPos() {
    this->currentReadNode = root;
    this->currentReadPos = 0;
}

bool AbstractByteBuffer::eof() {
    if (currentReadNode->length - currentReadPos) {
        return false;
    }
    if (currentReadNode->next &&
        currentReadNode->next->length) {
        return false;
    }
    return true;
}

size_t AbstractByteBuffer::getLength() const {
    return this->length + currentWriteNode->length;
}

size_t AbstractByteBuffer::getCapacity() const {
    return this->cap;
}

size_t AbstractByteBuffer::getReadableSize() const {
    size_t size = currentReadNode->length - currentReadPos;
    auto p_node = currentReadNode->next;
    while (p_node) {
        if (p_node == currentWriteNode) {
            size += currentWritePos;
        } else {
            size += p_node->cap;
        }
        p_node = p_node->next;
    }
    return size;
}

size_t AbstractByteBuffer::freeCapacity() {
    // 还原 root，删除 root 之后的节点
    size_t free_cap = 0;
    auto p_node = root->next;
    while (p_node) {
        const auto TO_DEL = p_node;
        free_cap += TO_DEL->cap;
        p_node = p_node->next;
        delete TO_DEL;
    }

    this->root->length = 0;
    this->root->next = nullptr;
    this->currentWriteNode = this->root;
    this->currentReadNode = this->root;
    this->currentWritePos = 0;
    this->currentReadPos = 0;

    return free_cap;
}

void AbstractByteBuffer::swap(AbstractByteBuffer &other) noexcept {
    std::swap(this->root, other.root);
    std::swap(this->currentWriteNode, other.currentWriteNode);
    std::swap(this->currentWritePos, other.currentWritePos);
    std::swap(this->currentReadNode, other.currentReadNode);
    std::swap(this->currentReadPos, other.currentReadPos);
    std::swap(this->length, other.length);
    std::swap(this->cap, other.cap);
}

int64_t AbstractByteBuffer::read(void *buffer, size_t need_read) {
    int64_t actual_read = 0;
    while (true) {
        // 当前单元能提供的剩余读取量
        // 当前单元能满足读取需求
        if (const size_t CURRENT_READ_NODE_REMAINING = currentReadNode->length - currentReadPos ;need_read <= CURRENT_READ_NODE_REMAINING) {
            memcpy(static_cast<char *>(buffer) + actual_read, static_cast<char *>(currentReadNode->buffer) + currentReadPos, need_read);
            actual_read += static_cast<int64_t>(need_read);
            currentReadPos += need_read;
            break;
        }
        // 当前单元不能满足读取需求
        else {
            memcpy(static_cast<char *>(buffer) + actual_read, static_cast<char *>(currentReadNode->buffer) + currentReadPos, CURRENT_READ_NODE_REMAINING);
            actual_read += static_cast<int64_t>(CURRENT_READ_NODE_REMAINING);
            need_read -= CURRENT_READ_NODE_REMAINING;
            currentReadPos += CURRENT_READ_NODE_REMAINING;
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
    return actual_read;
}

int64_t AbstractByteBuffer::write(const void *buffer, size_t need_write) {
    int64_t actual_write = 0;
    while (true) {
        // 当前单元能提供的剩余写入量
        // 当前单元能满足写入需求
        if (const size_t CURRENT_WRITE_NODE_REMAINING = currentWriteNode->cap - currentWriteNode->length; need_write <= CURRENT_WRITE_NODE_REMAINING) {
            memcpy(static_cast<char *>(currentWriteNode->buffer) + currentWritePos, static_cast<const char *>(buffer) + actual_write, need_write);
            actual_write += static_cast<int64_t>(need_write);
            currentWritePos += need_write;
            currentWriteNode->length += need_write;
            break;
        }
        // 当前单元不能满足读取需求
        else {
            memcpy(static_cast<char *>(currentWriteNode->buffer) + currentWritePos, static_cast<const char *>(buffer) + actual_write, CURRENT_WRITE_NODE_REMAINING);
            actual_write += static_cast<int64_t>(CURRENT_WRITE_NODE_REMAINING);
            need_write -= CURRENT_WRITE_NODE_REMAINING;
            currentWriteNode->length = currentWriteNode->cap;

            // 更新全局信息
            length += currentWriteNode->cap;
            cap += factor;
            // 直接切换至下一个单元
            currentWritePos = 0;
            currentWriteNode->next = new Node(factor); // GCOVR_EXCL_LINE
            currentWriteNode = currentWriteNode->next;
            continue;
        }
    }
    return actual_write;
}

int64_t AbstractByteBuffer::peek(void *buffer, size_t need_read) {
    const Node *current_read_node = currentReadNode;
    const Node *current_write_node = currentWriteNode;
    auto current_read_pos = currentReadPos;

    int64_t actual_read = 0;
    while (true) {
        // 当前单元能提供的剩余读取量
        // 当前单元能满足读取需求
        if (const size_t CURRENT_READ_NODE_REMAINING = current_read_node->length - current_read_pos ;need_read <= CURRENT_READ_NODE_REMAINING) {
            memcpy(static_cast<char *>(buffer) + actual_read, static_cast<char *>(current_read_node->buffer) + current_read_pos, need_read);
            actual_read += static_cast<int64_t>(need_read);
            current_read_pos += need_read;
            break;
        }
        // 当前单元不能满足读取需求
        else {
            memcpy(static_cast<char *>(buffer) + actual_read, static_cast<char *>(current_read_node->buffer) + current_read_pos, CURRENT_READ_NODE_REMAINING);
            actual_read += static_cast<int64_t>(CURRENT_READ_NODE_REMAINING);
            need_read -= CURRENT_READ_NODE_REMAINING;
            current_read_pos += CURRENT_READ_NODE_REMAINING;
            if (current_read_node == current_write_node) {
                // 已经没有剩余节点可供读取
                break;
            } else {
                // 切换下一个节点继续读取
                current_read_node = current_read_node->next;
                current_read_pos = 0;
                continue;
            }
        }
    }
    return actual_read;
}

int64_t AbstractByteBuffer::trunc(size_t need_read) {
    int64_t actual_read = 0;
    while (true) {
        // 当前单元能提供的剩余读取量
        // 当前单元能满足读取需求
        if (const size_t CURRENT_READ_NODE_REMAINING = currentReadNode->length - currentReadPos ;need_read <= CURRENT_READ_NODE_REMAINING) {
            // memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, length);
            actual_read += static_cast<int64_t>(need_read);
            currentReadPos += need_read;
            break;
        }
        // 当前单元不能满足读取需求
        else {
            // memcpy((char *) buffer + actualRead, (char *) currentReadNode->buffer + currentReadPos, currentReadNodeRemaining);
            actual_read += static_cast<int64_t>(CURRENT_READ_NODE_REMAINING);
            need_read -= CURRENT_READ_NODE_REMAINING;
            currentReadPos += CURRENT_READ_NODE_REMAINING;
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
    return actual_read;
}

} // namespace sese::io