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

#include "sese/io/AbstractFixedBuffer.h"

sese::io::AbstractFixedBuffer::AbstractFixedBuffer(size_t size) noexcept
    : size(size),
      buffer(new char[size]) { // GCOVR_EXCL_LINE
}

sese::io::AbstractFixedBuffer::~AbstractFixedBuffer() noexcept {
    if (buffer) { // NOLINT
        delete[] buffer;
    }
}

sese::io::AbstractFixedBuffer::AbstractFixedBuffer(const sese::io::AbstractFixedBuffer &buffer) noexcept {
    size = buffer.size;
    readSize = buffer.readSize;
    writeSize = buffer.writeSize;
    this->buffer = new char[size];
    memcpy(this->buffer, buffer.buffer, size);
}

sese::io::AbstractFixedBuffer::AbstractFixedBuffer(sese::io::AbstractFixedBuffer &&buffer) noexcept {
    this->size = buffer.size;
    this->readSize = buffer.readSize;
    this->writeSize = buffer.writeSize;
    this->buffer = buffer.buffer;

    buffer.size = 0;
    buffer.readSize = 0;
    buffer.writeSize = 0;
    buffer.buffer = nullptr;
}

int64_t sese::io::AbstractFixedBuffer::read(void *buf, size_t length) {
    if (writeSize - readSize >= length) {
        memcpy(buf, buffer + readSize, length);
        readSize += length;
        return static_cast<int64_t>(length);
    } else {
        auto read = writeSize - readSize;
        memcpy(buf, buffer + readSize, read);
        readSize += read;
        return static_cast<int64_t>(read);
    }
}

int64_t sese::io::AbstractFixedBuffer::write(const void *buf, size_t length) {
    if (size - writeSize >= length) {
        memcpy(buffer + writeSize, buf, length);
        writeSize += length;
        return static_cast<int64_t>(length);
    } else {
        auto write = size - writeSize;
        memcpy(buffer + writeSize, buf, write);
        writeSize += write;
        return static_cast<int64_t>(write);
    }
}

int64_t sese::io::AbstractFixedBuffer::peek(void *buf, size_t length) {
    if (writeSize - readSize >= length) {
        memcpy(buf, buffer + readSize, length);
        // readSize += length;
        return static_cast<int64_t>(length);
    } else {
        auto read = writeSize - readSize;
        memcpy(buf, buffer + readSize, read);
        // readSize += read;
        return static_cast<int64_t>(read);
    }
}

void sese::io::AbstractFixedBuffer::reset() noexcept {
    readSize = writeSize = 0;
}

int64_t sese::io::AbstractFixedBuffer::trunc(size_t length) {
    if (writeSize - readSize >= length) {
        // memcpy(buf, buffer + readSize, length);
        readSize += length;
        return static_cast<int64_t>(length);
    } else {
        auto read = writeSize - readSize;
        // memcpy(buf, buffer + readSize, read);
        readSize += read;
        return static_cast<int64_t>(read);
    }
}
