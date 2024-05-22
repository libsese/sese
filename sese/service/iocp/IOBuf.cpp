#include "sese/service/iocp/IOBuf.h"
#include "sese/record/Marco.h"

void *sese::iocp::IOBufNode::operator new(size_t size) {
    auto p = malloc(size);
    memset(p, 0, size);
    // SESE_DEBUG("malloc %p", p);
    return p;
}

void sese::iocp::IOBufNode::operator delete(void *p) {
    // SESE_DEBUG("free %p", p);
    free(p);
}

sese::iocp::IOBufNode::IOBufNode(size_t capacity) : CAPACITY(capacity) {
    buffer = malloc(capacity);
}

sese::iocp::IOBufNode::~IOBufNode() {
    if (buffer) {
        free(buffer);
    }
}

size_t sese::iocp::IOBufNode::getReadableSize() const noexcept {
    return size - read;
}

size_t sese::iocp::IOBufNode::getWriteableSize() const noexcept {
    return CAPACITY - size;
}

sese::iocp::IOBuf::~IOBuf() {
    if (root) {
        clear();
    }
}

void sese::iocp::IOBuf::push(sese::iocp::IOBuf::Node *node) {
    if (!tail) {
        root = tail = cur = node;
    } else {
        tail->next = node;
        tail = node;
    }
    total += node->size;
}

void sese::iocp::IOBuf::clear() {
    Node *p_cur = root;
    while (p_cur) {
        auto next = p_cur->next;
        delete p_cur;
        p_cur = next;
    }
    root = tail = cur = nullptr;
    total = 0;
    readed = 0;
}

size_t sese::iocp::IOBuf::getReadableSize() const noexcept {
    return total - readed;
}

size_t sese::iocp::IOBuf::getTotalSize() const noexcept {
    return total;
}

int64_t sese::iocp::IOBuf::read(void *buffer, size_t length) {
    size_t real = 0;
    while (true) {
        size_t node_remaining = cur->getReadableSize();
        if (length <= node_remaining) {
            memcpy(
                    static_cast<char *>(buffer) + real,
                    static_cast<char *>(cur->buffer) + cur->read,
                    length
            );
            real += length;
            cur->read += length;
            readed += length;
            break;
        } else {
            memcpy(
                    static_cast<char *>(buffer) + real,
                    static_cast<char *>(cur->buffer) + cur->read,
                    node_remaining
            );
            real += node_remaining;
            cur->read += node_remaining;
            readed += node_remaining;
            if (cur == tail) {
                break;
            } else {
                cur = cur->next;
                length -= node_remaining;
            }
        }
    }
    return static_cast<int64_t>(real);
}

int64_t sese::iocp::IOBuf::peek(void *buffer, size_t length) {
    size_t real = 0;
    Node *my_cur = cur;
    Node node = *my_cur;
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
            if (my_cur == tail) {
                break;
            } else {
                my_cur = my_cur->next;
                node.buffer = my_cur->buffer;
                node.size = my_cur->size;
                node.read = 0;
                length -= node_remaining;
            }
        }
    }
    node.buffer = nullptr;
    return static_cast<int64_t>(real);
}

int64_t sese::iocp::IOBuf::trunc(size_t length) {
    size_t real = 0;
    while (true) {
        size_t node_remaining = cur->getReadableSize();
        if (length <= node_remaining) {
            real += length;
            cur->read += length;
            readed += length;
            break;
        } else {
            real += node_remaining;
            cur->read += node_remaining;
            readed += node_remaining;
            if (cur == tail) {
                break;
            } else {
                cur = cur->next;
                length -= node_remaining;
            }
        }
    }
    return static_cast<int64_t>(real);
}
