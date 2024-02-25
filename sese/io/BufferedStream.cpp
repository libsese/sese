#include "sese/io/BufferedStream.h"
#include <cstring>

using sese::io::BufferedStream;

BufferedStream::BufferedStream(const Stream::Ptr &source, size_t buffer_size) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = buffer_size;
    this->buffer = malloc(cap);
}

BufferedStream::~BufferedStream() noexcept {
    free(buffer);
}

inline int64_t BufferedStream::preRead() {
    // 尝试使用目标流填充缓存
    auto read = source->read(buffer, cap);
    // 此处用于修正一些输入源可能读取返回负值的情况
    read = read < 0 ? 0 : read; // GCOVR_EXCL_LINE
    pos = 0;
    len = read;
    return read;
}

void BufferedStream::clear() noexcept {
    // 清除所有标志位
    pos = 0;
    len = 0;
}

int64_t BufferedStream::read(void *buf, size_t length) {
    /*
     * 如果读取所需字节数需要缓存两次以下，
     * 需要进行预读操作，避免频繁的IO操作；
     * 如果读取所需字节数需要缓存两次及两次以上，
     * 则处理原先的缓存后，直接操作裸流，减少拷贝次数
     */
    if (length <= this->cap) {
        if (this->len - this->pos >= length) {
            // 字节数足够 - 不需要预读取
            memcpy(buf, static_cast<char *>(this->buffer) + this->pos, length);
            pos += length;
            return static_cast<int64_t>(length);
        } else {
            // 字节数不足 - 需要预读取
            size_t total = this->len - this->pos;
            memcpy(buf, static_cast<char *>(this->buffer) + this->pos, total);
            pos += total;
            if (0 != preRead()) {
                if (this->len - this->pos >= length - total) {
                    // 字节数足够
                    memcpy(static_cast<char *>(buf) + total, this->buffer, length - total);
                    pos = length - total;
                    total = length;
                } else {
                    // 字节数不足，且无法继续读取
                    memcpy(static_cast<char *>(buf) + total, this->buffer, this->len - this->pos);
                    pos = this->len - this->pos;
                    total += this->len - this->pos;
                }
            }
            return static_cast<int64_t>(total);
        }
    } else {
        // 先处理已有缓存
        size_t total = this->len - this->pos;
        memcpy(buf, this->buffer, total);
        this->len = 0;
        this->pos = 0;
        // 操作裸流
        while (true) {
            size_t read = source->read(static_cast<char *>(buf) + total, (length - total) >= 1024 ? 1024 : length - total);
            total += static_cast<int64_t>(read);
            // 无可再读
            if (read <= 0) break;
            // 完成目标
            if (total == length) break;
        }
        return static_cast<int64_t>(total);
    }
}

int64_t BufferedStream::write(const void *buf, size_t length) {
    /*
     * 如果写入所需字节数需要刷新缓存两次一下，
     * 则暂时写入缓存，避免频繁的IO操作；
     * 如果写入所需字节数需要刷新缓存两次及两次以上，
     * 则处理原先的缓存后，直接操作裸流，减少拷贝次数
     */
    if (length <= this->cap) {
        if (this->cap - this->len >= length) {
            // 字节数足够 - 不需要刷新
            memcpy(static_cast<char *>(this->buffer) + this->len, buf, length);
            this->len += length;
            return static_cast<int64_t>(length);
        } else {
            // 字节数不足 - 需要刷新
            size_t expect = len - pos;
            if (expect == flush()) {
                memcpy(this->buffer, (char *) buf, length);
                this->len = length;
                expect = length;
                return static_cast<int64_t>(expect);
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
            auto rt = source->write(static_cast<const char *>(buf) + wrote, length - wrote >= cap ? cap : length - wrote);
            if (rt <= 0) return -1;
            wrote += rt;
            if (wrote == length) break;
        }

        return wrote;
    }
}

int64_t BufferedStream::flush() noexcept {
    // 将已有未处理数据立即写入流
    auto wrote = source->write(static_cast<char *>(buffer) + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}

void BufferedStream::reset(const sese::io::Stream::Ptr &new_source) noexcept {
    this->source = new_source;
    pos = 0;
    len = 0;
}
