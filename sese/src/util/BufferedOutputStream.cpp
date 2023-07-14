#include "sese/util/BufferedOutputStream.h"

using sese::BufferedOutputStream;

BufferedOutputStream::BufferedOutputStream(const OutputStream::Ptr &source, size_t bufferSize) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = bufferSize;
    this->buffer = malloc(cap);
}

BufferedOutputStream::~BufferedOutputStream() noexcept {
    free(buffer);
}

int64_t BufferedOutputStream::write(const void *buf, size_t length) {
    /*
     * 如果写入所需字节数需要刷新缓存两次一下，
     * 则暂时写入缓存，避免频繁的IO操作；
     * 如果写入所需字节数需要刷新缓存两次及两次以上，
     * 则处理原先的缓存后，直接操作裸流，减少拷贝次数
     */
    if (length <= this->cap) {
        if (this->cap - this->len >= length) {
            // 字节数足够 - 不需要刷新
            memcpy((char *) this->buffer + this->len, buf, length);
            this->len += length;
            return (int64_t) length;
        } else {
            // 字节数不足 - 需要刷新
            size_t expect = len - pos;
            if (expect == flush()) {
                memcpy(this->buffer, (char *) buf, length);
                this->len = length;
                expect = length;
                return (int64_t) expect;
            } else {
                // flush 失败
                return -1;
            }
        }
    } else {
        // 直接写入
        if (this->len != this->pos) {
            // 缓存区有剩余，需要刷新
            size_t expect = len - pos;
            if (expect != flush()) {
                // flush 失败
                return -1;
            }
        }

        int64_t wrote = 0;
        while (true) {
            auto rt = source->write((const char *) buf + wrote, length - wrote >= cap ? cap : length - wrote);
            if (rt == 0) {
                return -1;
            } else {

            }
            wrote += rt;
            if (wrote == length) break;
        }

        return wrote;
    }
}

int64_t BufferedOutputStream::flush() noexcept {
    // 将已有未处理数据立即写入流
    auto wrote = source->write((char *) buffer + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}