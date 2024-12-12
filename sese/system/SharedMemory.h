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

/**
 * \file SharedMemory.h
 * \brief Shared Memory Class
 * \author kaoru
 * \date December 8, 2022
 * \version 0.1
 */
#pragma once

#include <sese/Config.h>
#include <sese/util/Result.h>
#include <sese/util/ErrorCode.h>
#ifdef WIN32
#else
#include <sys/shm.h>
#endif

namespace sese::system {

/// \brief Shared Memory Class
class SharedMemory final {
public:
    using Ptr = std::unique_ptr<SharedMemory>;

    /// Create a shared memory block
    /// \param name Shared memory name
    /// \param size Shared memory size
    /// \return Pointer to the shared memory object
    /// \retval nullptr Creation failed
    static SharedMemory::Ptr create(const char *name, size_t size) noexcept;

    static Result<Ptr, ErrorCode> createEx(const char *name, size_t size) noexcept;

    /// Use an existing shared memory block
    /// \param name Shared memory name
    /// \return Pointer to the shared memory object
    /// \retval nullptr Acquisition failed
    static SharedMemory::Ptr use(const char *name) noexcept;

    static Result<Ptr, ErrorCode> useEx(const char *name) noexcept;

    /// Destructor
    ~SharedMemory() noexcept;
    /// Get the actual address of the shared memory
    /// \return Shared memory block
    void *getBuffer() noexcept;

private:
    SharedMemory() = default;

private:
#ifdef WIN32
    void *hMapFile = nullptr;
#else
    int id{};
    static key_t name2key(const char *name) noexcept;
#endif
    void *buffer = nullptr;
    bool isOwner{};
};
} // namespace sese::system
