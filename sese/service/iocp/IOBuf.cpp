#include "sese/service/iocp/IOBuf.h"

sese::iocp::IOBufNode::IOBufNode(size_t capacity) : capacity(capacity) {
    buffer = malloc(capacity);
}

sese::iocp::IOBufNode::~IOBufNode() {
    free(buffer);
}

size_t sese::iocp::IOBufNode::getReadableSize() const noexcept {
    return size - read;
}

size_t sese::iocp::IOBufNode::getWriteableSize() const noexcept {
    return capacity - size;
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
    Node *pCur = root;
    while (pCur) {
        auto next = pCur->next;
        delete pCur;
        pCur = next;
    }
    root = tail = cur = nullptr;
    total = 0;
    readed = 0;
}

size_t sese::iocp::IOBuf::getReadableSize() const noexcept {
    return total - readed;
}

int64_t sese::iocp::IOBuf::read(void *buffer, size_t length) {
    size_t real = 0;
    while (true) {
        size_t nodeRemaining = cur->getReadableSize();
        if (length <= nodeRemaining) {
            memcpy(
                    (char *) buffer + real,
                    (char *) cur->buffer + cur->read,
                    length
            );
            real += length;
            cur->read += length;
            readed += length;
            break;
        } else {
            memcpy(
                    (char *) buffer + real,
                    (char *) cur->buffer + cur->read,
                    nodeRemaining
            );
            real += nodeRemaining;
            cur->read += nodeRemaining;
            readed += nodeRemaining;
            if (cur == tail) {
                break;
            } else {
                cur = cur->next;
            }
        }
    }
    return (int64_t) real;
}
