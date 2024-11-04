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

#include <sese/system/SharedMemory.h>
#include <sese/io/ByteBuilder.h>
#include <sese/convert/MD5Util.h>

using namespace sese::system;

key_t SharedMemory::name2key(const char *name) noexcept {
    auto builder = std::make_shared<io::ByteBuilder>();
    builder->write(name, strlen(name));
    auto str = MD5Util::encode(builder);

    char num[]{
            str[1],
            str[3],
            str[2],
            str[5],
            str[7],
            str[11],
            str[13],
            0};
    std::size_t size;
    return std::stoi(num, &size, 16);
}

SharedMemory::~SharedMemory() noexcept {
    shmdt(buffer);
    if (isOwner) {
        shmctl(id, IPC_RMID, nullptr);
    }
}

SharedMemory::Ptr SharedMemory::create(const char *name, size_t size) noexcept {
    int32_t key = name2key(name);
    int32_t id = shmget((key_t) key, size, 0666 | IPC_CREAT);
    if (-1 == id) {
        return nullptr;
    }

    auto buffer = shmat(id, nullptr, 0);
    if (buffer == (void *) -1) {
        return nullptr;
    }

    auto rt = MAKE_UNIQUE_PRIVATE(SharedMemory);
    rt->isOwner = true;
    rt->id = id;
    rt->buffer = buffer;
    return rt;
}

SharedMemory::Ptr SharedMemory::use(const char *name) noexcept {
    int32_t key = name2key(name);
    int32_t id = shmget(key, 0, 0666);
    if (-1 == id) {
        return nullptr;
    }

    auto buffer = shmat(id, nullptr, 0);
    if (buffer == (void *) -1) {
        return nullptr;
    }

    auto rt = MAKE_UNIQUE_PRIVATE(SharedMemory);
    rt->isOwner = false;
    rt->id = id;
    rt->buffer = buffer;
    return rt;
}

void *SharedMemory::getBuffer() noexcept {
    return buffer;
}