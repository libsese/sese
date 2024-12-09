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

#include <sese/res/ResourceStream.h>

sese::res::ResourceStream::ResourceStream(const void *buf, size_t size) : data(buf), size(size) {}

int64_t sese::res::ResourceStream::read(void *buffer, size_t length) {
    auto readable = size - pos;
    if (readable > 0) {
        memcpy(buffer, static_cast<const char *>(data) + pos, readable);
        pos += readable;
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::peek(void *buffer, size_t length) {
    auto readable = size - pos;
    if (readable > 0) {
        memcpy(buffer, static_cast<const char *>(data) + pos, readable);
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::trunc(size_t length) {
    auto readable = size - pos;
    if (readable > 0) {
        pos += readable;
    }
    return static_cast<int64_t>(readable);
}

int64_t sese::res::ResourceStream::getSeek() const {
    return static_cast<int64_t>(pos);
}

int32_t sese::res::ResourceStream::setSeek(int64_t offset, io::Seek seek) {
    return setSeek(offset, static_cast<int32_t>(seek));
}

int32_t sese::res::ResourceStream::setSeek(int64_t offset, int32_t whence) {
    int64_t new_pos;
    if (whence == SEEK_CUR) {
        new_pos = static_cast<int64_t>(pos) + offset;
    } else if (whence == SEEK_SET) {
        new_pos = offset;
    } else if (whence == SEEK_END) {
        new_pos = static_cast<int64_t>(size) + offset;
    } else {
        return -1;
    }
    if (new_pos < 0 || new_pos - 1 > static_cast<int64_t>(size)) {
        return -1;
    }
    pos = static_cast<size_t>(new_pos);
    return 0;
}
