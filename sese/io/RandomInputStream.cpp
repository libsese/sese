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

#include <sese/io/RandomInputStream.h>
#include <sese/util/RandomUtil.h>

#include <memory.h>

int64_t sese::io::RandomInputStream::read(void *buffer, size_t length) {
    size_t count = 0;
    while (count != length) {
        auto r = RandomUtil::next<uint64_t>();
        auto need = std::min<size_t>(8, length - count);
        memcpy(static_cast<uint8_t *>(buffer) + count, &r, need);
        count += need;
    }
    return static_cast<int64_t>(length);
}
