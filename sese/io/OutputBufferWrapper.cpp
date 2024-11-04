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

#include "sese/io/OutputBufferWrapper.h"

const char *sese::io::OutputBufferWrapper::getBuffer() const {
    return buffer;
}

size_t sese::io::OutputBufferWrapper::getLength() const {
    return len;
}

size_t sese::io::OutputBufferWrapper::getCapacity() const {
    return cap;
}

sese::io::OutputBufferWrapper::OutputBufferWrapper(char *buffer, size_t cap) {
    this->buffer = buffer;
    this->cap = cap;
}

int64_t sese::io::OutputBufferWrapper::write(const void *buf, size_t length) {
    auto remain = cap - len;
    if (remain == 0) {
        return 0;
    } else if (remain >= length) {
        memcpy(buffer + len, buf, length);
        len += length;
        return static_cast<int64_t>(length);
    } else {
        memcpy(buffer + len, buf, remain);
        len = cap;
        return static_cast<int64_t>(remain);
    }
}

void sese::io::OutputBufferWrapper::reset() noexcept {
    this->len = 0;
}