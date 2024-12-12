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

/// Indicates basic information of shared memory
struct MemInfo {
    /// The total size of the shared memory, excluding the size of this structure
    size_t total_size;
    /// The enqueue position of the queue elements, excluding the offset of this structure
    size_t tail;
};

/// Message entity
class Message {
public:
    /// Instantiate message structure
    /// \param message buffer
    explicit Message(std::string message);

    /// Return current data as a string
    /// \return String
    [[nodiscard]] const std::string &getDataAsString() const;

    /// Get data pointer
    /// \return Data
    [[nodiscard]] const void *data() const;

    /// Get data length
    /// \return Data length
    [[nodiscard]] size_t length() const;

private:
    std::string message;
};

/// Message iterator
struct MessageIterator {
    uint32_t id;
    uint32_t length;
    void *buffer;
};

/// IPC Channel
class IPCChannel {
public:
    using Ptr = std::unique_ptr<IPCChannel>;

    /// Create an IPC channel
    /// \param name Channel name
    /// \param buffer_size Size of the shared memory
    /// \return IPC channel
    static IPCChannel::Ptr create(const std::string &name, size_t buffer_size);

    static Result<Ptr, ErrorCode> createEx(const std::string &name, size_t buffer_size);

    /// Use an existing IPC channel
    /// \param name Channel name
    /// \return IPC channel
    static IPCChannel::Ptr use(const std::string &name);

    static Result<Ptr, ErrorCode> useEx(const std::string &name);

    /// Write data to the IPC channel with the specified id
    /// \param id Identifier
    /// \param data Data
    /// \param length Data size
    /// \return Whether the write was successful
    bool write(uint32_t id, const void *data, uint32_t length);

    /// Write data to the IPC channel with the specified id
    /// \param id Identifier
    /// \param message Data
    /// \return Whether the write was successful
    bool write(uint32_t id, const std::string &message);

    /// Read data from the IPC channel with the specified id
    /// \param id Identifier
    /// \return Data set
    std::list<Message> read(uint32_t id);

private:
    IPCChannel() = default;

    MemInfo *mem_info{};
    void *buffer{};
    Semaphore::Ptr semaphore{};
    SharedMemory::Ptr shared_memory{};
};

}