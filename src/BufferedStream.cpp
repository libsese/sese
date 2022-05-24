#include <sese/BufferedStream.h>

using sese::BufferedStream;

BufferedStream::BufferedStream(const Stream::Ptr &source, size_t bufferSize) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = bufferSize;
    this->buffer = malloc(cap);
}

BufferedStream::~BufferedStream() noexcept {
    free(buffer);
}

int64_t BufferedStream::preRead() {
    // 尝试使用目标流填充缓存
    auto read = source->read(buffer, cap);
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
            memcpy(buf, (char *) this->buffer + this->pos, length);
            pos += length;
            return (int64_t) length;
        } else {
            // 字节数不足 - 需要预读取
            size_t total = this->len - this->pos;
            memcpy(buf, (char *) this->buffer + this->pos, total);
            pos += total;
            if (0 != preRead()) {
                if (this->len - this->pos >= length - total) {
                    // 字节数足够
                    memcpy((char *) buf + total, this->buffer, length - total);
                    pos = length - total;
                    total = length;
                } else {
                    // 字节数不足，且无法继续读取
                    memcpy((char *) buf + total, this->buffer, this->len - this->pos);
                    pos = this->len - this->pos;
                    total += this->len - this->pos;
                }
            }
            return (int64_t) total;
        }
    } else {
        // 先处理已有缓存
        size_t total = this->len - this->pos;
        memcpy(buf, this->buffer, total);
        this->cap = 0;
        this->len = 0;
        // 操作裸流
        size_t read;
        while (true) {
            read = source->read((char *) buf + total, this->cap);
            total += (int64_t) read;
            if (read != this->cap || total == length) {
                // 流已读尽或者需求已满足则退出
                break;
            }
        }
        return (int64_t) total;
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
            memcpy((char *) this->buffer + this->len, buf, length);
            this->len += length;
            return (int64_t) length;
        } else {
            // 字节数不足 - 需要刷新
            size_t total = flush();
            if (0 != total) {
                memcpy(this->buffer, (char *) buf, length);
                this->len = length;
                total = length;
            }
            return (int64_t) total;
        }
    } else {
        // 处理已有缓存
        size_t total = flush();
        if (0 != total) {
            total = 0;
            size_t wrote = 0;
            while(true) {
                size_t need;
                if(length - total >= this->cap) {
                    need = this->cap;
                }else {
                    need = length - total;
                }
                wrote = source->write((char *)buffer + total, need);
                total += wrote;
                if(0 == wrote || total == length) {
                    // 流已无法继续写入或需求已满足则退出
                    break;
                }
            }
        }
        return (int64_t) total;
    }
}

int64_t BufferedStream::flush() noexcept {
    // 将已有未处理数据立即写入流
    auto wrote = source->write((char *) buffer + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}

void BufferedStream::close() {}