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

#include <sese/system/IPC.h>
#include <sese/thread/Locker.h>

using sese::system::IPCChannel;
using sese::system::MemInfo;
using sese::system::Message;
using sese::system::MessageIterator;
using sese::system::Semaphore;
using sese::system::SharedMemory;

Message::Message(std::string message) : message(std::move(message)) {}

const std::string &Message::getDataAsString() const {
    return message;
}

const void *Message::data() const {
    return message.data();
}

size_t Message::length() const {
    return message.length();
}

IPCChannel::Ptr IPCChannel::create(const std::string &name, size_t buffer_size) {
    auto sem_name = name + "00Semaphore";
    auto mem_name = name + "00Memory";

    auto sem = Semaphore::create(sem_name);
    if (!sem) {
        return nullptr;
    }

    sem->lock();
    auto mem = SharedMemory::create(mem_name.c_str(), buffer_size + sizeof(MemInfo));
    if (!mem) {
        return nullptr;
    }

    auto buffer = static_cast<char *>(mem->getBuffer());
    auto mem_info = reinterpret_cast<MemInfo *>(buffer + 0);
    memset(mem_info, 0, sizeof(MemInfo));
    mem_info->total_size = buffer_size;
    mem_info->tail = 0;

    auto first_message = reinterpret_cast<MemInfo *>(buffer + sizeof(uint32_t));
    memset(first_message, 0, 4);

    sem->unlock();

    auto ipc = MAKE_UNIQUE_PRIVATE(IPCChannel);
    ipc->mem_info = mem_info;
    ipc->buffer = buffer + sizeof(MemInfo);
    ipc->semaphore = std::move(sem);
    ipc->shared_memory = std::move(mem);
    return ipc;
}

sese::Result<IPCChannel::Ptr> IPCChannel::createEx(const std::string &name, size_t buffer_size) {
    if (auto result = create(name, buffer_size)) {
        return std::move(result);
    }
    return Result<Ptr>::fromLastError();
}

IPCChannel::Ptr IPCChannel::use(const std::string &name) {
    auto sem_name = name + "00Semaphore";
    auto mem_name = name + "00Memory";

    auto sem = Semaphore::create(sem_name);
    if (!sem) {
        return nullptr;
    }

    auto mem = SharedMemory::use(mem_name.c_str());
    if (!mem) {
        return nullptr;
    }

    auto buffer = static_cast<char *>(mem->getBuffer());
    auto mem_info = reinterpret_cast<MemInfo *>(buffer + 0);

    auto ipc = MAKE_UNIQUE_PRIVATE(IPCChannel);
    ipc->mem_info = mem_info;
    ipc->buffer = buffer + sizeof(MemInfo);
    ipc->semaphore = std::move(sem);
    ipc->shared_memory = std::move(mem);
    return ipc;
}

sese::Result<IPCChannel::Ptr> IPCChannel::useEx(const std::string &name) {
    if (auto result = use(name)) {
        return std::move(result);
    }
    return Result<Ptr>::fromLastError();
}

#define HEADER_SIZE (sizeof(MessageIterator) - sizeof(uint32_t) * 2)

bool IPCChannel::write(uint32_t id, const void *data, uint32_t length) {
    Locker locker(*semaphore);
    // 确认共享内存空间是否足够入队一条信息
    if (mem_info->tail + HEADER_SIZE + length > mem_info->total_size + 4) {
        return false;
    }

    // 拷贝信息至共享内存
    auto buf = static_cast<char *>(buffer);
    memcpy(buf + mem_info->tail + 0, &id, sizeof(id));
    memcpy(buf + mem_info->tail + 4, &length, sizeof(length));
    memcpy(buf + mem_info->tail + 8, data, length);

    // 更新头部内存信息
    mem_info->tail += 8 + length;
    return true;
}

#undef HEADER_SIZE

bool IPCChannel::write(uint32_t id, const std::string &message) {
    return write(id, message.data(), static_cast<uint32_t>(message.length()));
}

std::list<Message> IPCChannel::read(uint32_t id) {
    std::list<Message> list;
    std::list<MessageIterator> iterators;
    Locker locker(*semaphore);

    auto buf = static_cast<char *>(this->buffer);
    MessageIterator message{};
    while (true) {
        memcpy(&message.id, buf + 0, sizeof(uint32_t));
        if (message.id == 0) {
            break;
        } else {
            memcpy(&message.length, buf + 4, sizeof(uint32_t));
            if (message.length) {
                message.buffer = buf + 8;
                iterators.push_back(message);
                buf += 8 + message.length;
            } else {
                message.buffer = nullptr;
                iterators.push_back(message);
                buf += 8;
            }
        }
    }

    size_t size = 0;
    buf = static_cast<char *>(this->buffer);
    for (auto && iterator = iterators.begin(); iterator != iterators.end(); ) {
        if (iterator->id == id) {
            list.emplace_back(std::string(static_cast<char *>(iterator->buffer), iterator->length));
            iterator = iterators.erase(iterator);
        } else {
            memcpy(buf + 0, &iterator->id, sizeof(uint32_t));
            memcpy(buf + 4, &iterator->length, sizeof(uint32_t));
            memmove(buf + 8, iterator->buffer, iterator->length);
            buf += 8 + iterator->length;
            size += 8 + iterator->length;
            iterator++;
        }
    }
    memset(buf, 0, sizeof(uint32_t));
    this->mem_info->tail = size;

    return list;
}