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

#include "IOBuf.h"

sese::IOBufNode::IOBufNode(size_t capacity) : CAPACITY(capacity) {
    buffer = malloc(capacity);
}

sese::IOBufNode::~IOBufNode() {
    if (buffer) {
        free(buffer);
    }
}

size_t sese::IOBufNode::getReadableSize() const noexcept {
    return size - read;
}

size_t sese::IOBufNode::getWriteableSize() const noexcept {
    return CAPACITY - size;
}

void sese::IOBuf::push(Node node) {
    // if (!tail) {
    //     root = tail = cur = node;
    // } else {
    //     tail->next = node;
    //     tail = node;
    // }
    // total += node->size;
    total += node->size;
    if (list.empty()) {
        list.push_back(std::move(node));
        cur = list.begin();
    } else {
        list.push_back(std::move(node));
    }
}

void sese::IOBuf::clear() {
    // Node *p_cur = root;
    // while (p_cur) {
    //     auto next = p_cur->next;
    //     delete p_cur;
    //     p_cur = next;
    // }
    // root = tail = cur = nullptr;
    // total = 0;
    // readed = 0;
    list.clear();
    total = 0;
    readed = 0;
}

size_t sese::IOBuf::getReadableSize() const noexcept {
    return total - readed;
}

size_t sese::IOBuf::getTotalSize() const noexcept {
    return total;
}

int64_t sese::IOBuf::read(void *buffer, size_t length) {
    size_t real = 0;
    while (true) {
        size_t node_remaining = cur->get()->getReadableSize();
        if (length <= node_remaining) {
            memcpy(
                    static_cast<char *>(buffer) + real,
                    static_cast<char *>(cur->get()->buffer) + cur->get()->read,
                    length
            );
            real += length;
            cur->get()->read += length;
            readed += length;
            break;
        }
        memcpy(
                static_cast<char *>(buffer) + real,
                static_cast<char *>(cur->get()->buffer) + cur->get()->read,
                node_remaining
        );
        real += node_remaining;
        cur->get()->read += node_remaining;
        readed += node_remaining;
        length -= node_remaining;
        ++cur;
        if (cur == list.end()) {
            break;
        }
    }
    return static_cast<int64_t>(real);
}

int64_t sese::IOBuf::peek(void *buffer, size_t length) {
    size_t real = 0;
    auto my_cur = cur;
    IOBufNode node = *(my_cur->get());
    while (true) {
        size_t node_remaining = node.getReadableSize();
        if (length <= node_remaining) {
            memcpy(
                    static_cast<char *>(buffer) + real,
                    static_cast<char *>(node.buffer) + node.read,
                    length
            );
            real += length;
            node.read += length;
            break;
        } else {
            memcpy(
                    static_cast<char *>(buffer) + real,
                    static_cast<char *>(node.buffer) + node.read,
                    node_remaining
            );
            real += node_remaining;
            node.read += node_remaining;
            ++my_cur;
            if (my_cur == list.end()) {
                break;
            }
            node.buffer = my_cur->get()->buffer;
            node.size = my_cur->get()->size;
            node.read = 0;
            length -= node_remaining;
        }
    }
    node.buffer = nullptr;
    return static_cast<int64_t>(real);
}

int64_t sese::IOBuf::trunc(size_t length) {
    size_t real = 0;
    while (true) {
        size_t node_remaining = cur->get()->getReadableSize();
        if (length <= node_remaining) {
            real += length;
            cur->get()->read += length;
            readed += length;
            break;
        }
        real += node_remaining;
        cur->get()->read += node_remaining;
        readed += node_remaining;
        if (cur == list.end()) {
            break;
        }
        ++cur;
        length -= node_remaining;
    }
    return static_cast<int64_t>(real);
}
