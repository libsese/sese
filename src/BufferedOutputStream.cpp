#include <sese/BufferedOutputStream.h>

using sese::BufferedOutputStream;

BufferedOutputStream::BufferedOutputStream(const Stream::Ptr &source, size_t bufferSize) {
    this->source = source;
    this->pos = 0;
    this->len = 0;
    this->cap = bufferSize;
    this->buffer = malloc(cap);
}

BufferedOutputStream::~BufferedOutputStream() noexcept {
    free(buffer);
}

int64_t BufferedOutputStream::flush() {
    // 将已有未处理数据立即写入流
    auto wrote = source->write((char *) buffer + pos, len - pos);
    pos = 0;
    len = 0;
    return wrote;
}

int64_t BufferedOutputStream::write(const void *buf, size_t length) noexcept {
    /*
     * 如果写入所需字节数需要刷新缓存两次一下，
     * 则暂时写入缓存，避免频繁的IO操作；
     * 如果写入所需字节数需要刷新缓存两次及两次以上，
     * 则处理原先的缓存后，直接操作裸流，减少拷贝次数
     */
}