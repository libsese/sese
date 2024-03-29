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

#pragma once

#include <sese/system/Semaphore.h>
#include <sese/system/SharedMemory.h>

#include <list>

namespace sese::system {

/// 指示共享内存的基本信息
struct MemInfo {
    /// 共享内存指示完整大小，不包含当前结构体的大小
    size_t total_size;
    /// 指示队列元素入队位置，不包含当前结构体的偏移
    size_t tail;
};

/// 消息实体
class Message {
public:
    explicit Message(std::string message);

    [[nodiscard]] const std::string &getDataAsString() const;

    [[nodiscard]] const void *data() const;

    [[nodiscard]] size_t length() const;

private:
    std::string message;
};

/// 信息迭代器
struct MessageIterator {
    uint32_t id;
    uint32_t length;
    void *buffer;
};

/// IPC 通道
class IPCChannel {
public:
    using Ptr = std::unique_ptr<IPCChannel>;

    static IPCChannel::Ptr create(const std::string &name, size_t buffer_size);

    /// 向 IPC 通道写入编号为 id 的数据
    /// \param id 编号
    /// \param data 数据
    /// \param length 数据大小
    /// \return 是否写入成功
    bool write(uint32_t id, const void *data, uint32_t length);

    /// 向 IPC 通道写入编号为 id 的数据
    /// \param id 编号
    /// \param message 数据
    /// \return
    bool write(uint32_t id, const std::string &message);

    /// 从 IPC 通道读取编号为 id 的信息
    /// \param id 编号
    /// \return 数据集合
    std::list<Message> read(uint32_t id);

private:
    IPCChannel() = default;

    MemInfo *mem_info{};
    void *buffer{};
    Semaphore::Ptr semaphore{};
    SharedMemory::Ptr shared_memory{};
};

}