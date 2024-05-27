#include "sese/service/iocp/IOBuf.h"
#include "sese/record/Marco.h"

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

void sese::iocp::IOBuf::push(Node node) {
    // if (!tail) {
    //     root = tail = cur = node;
    // } else {
    //     tail->next = node;
    //     tail = node;
    // }
    // total += node->size;
    if (list.empty()) {
        list.push_front(std::move(node));
        cur = list.begin();
    } else {
        list.push_front(std::move(node));
    }
    total += node->size;
}

void sese::iocp::IOBuf::clear() {
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
        if (cur == list.end()) {
            break;
        }
        ++cur;
        length -= node_remaining;
    }
    return static_cast<int64_t>(real);
}

int64_t sese::iocp::IOBuf::peek(void *buffer, size_t length) {
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
            if (my_cur == list.end()) {
                break;
            }
            ++my_cur;
            node.buffer = my_cur->get()->buffer;
            node.size = my_cur->get()->size;
            node.read = 0;
            length -= node_remaining;
        }
    }
    node.buffer = nullptr;
    return static_cast<int64_t>(real);
}

int64_t sese::iocp::IOBuf::trunc(size_t length) {
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
