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

#include "sese/io/ByteBuffer.h"
#include "sese/thread/Locker.h"

// GCOVR_EXCL_START

int64_t sese::io::ByteBuffer::read(void *buffer, size_t len) {
    Locker locker(mutex);
    return AbstractByteBuffer::read(buffer, len);
}

int64_t sese::io::ByteBuffer::write(const void *buffer, size_t need_write) {
    Locker locker(mutex);
    return AbstractByteBuffer::write(buffer, need_write);
}

int64_t sese::io::ByteBuffer::peek(void *buffer, size_t len) {
    Locker locker(mutex);
    return AbstractByteBuffer::peek(buffer, len);
}

sese::io::ByteBuffer::ByteBuffer(size_t base_size, size_t factor) : AbstractByteBuffer(base_size, factor) {
}

void sese::io::ByteBuffer::resetPos() {
    Locker locker(mutex);
    AbstractByteBuffer::resetPos();
}

bool sese::io::ByteBuffer::eof() {
    Locker locker(mutex);
    return AbstractByteBuffer::eof();
}

size_t sese::io::ByteBuffer::getLength() {
    Locker locker(mutex);
    return AbstractByteBuffer::getLength();
}

size_t sese::io::ByteBuffer::getCapacity() {
    Locker locker(mutex);
    return AbstractByteBuffer::getCapacity();
}

size_t sese::io::ByteBuffer::freeCapacity() {
    Locker locker(mutex);
    return AbstractByteBuffer::freeCapacity();
}

// size_t sese::ByteBuffer::getCurrentReadPos() {
//     Locker locker(mutex);
//     return AbstractByteBuffer::getCurrentReadPos();
// }
//
// size_t sese::ByteBuffer::getCurrentWritePos() {
//     Locker locker(mutex);
//     return AbstractByteBuffer::getCurrentWritePos();
// }

int64_t sese::io::ByteBuffer::trunc(size_t need_read) {
    Locker locker(mutex);
    return AbstractByteBuffer::trunc(need_read);
}

// GCOVR_EXCL_STOP
