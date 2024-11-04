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
 * \author kaoru
 * \date 2022.12.08
 * \brief 共享内存类
 * \version 0.1
 */
#pragma once

#include <sese/Config.h>
#include <sese/util/Result.h>
#ifdef WIN32
#else
#include <sys/shm.h>
#endif

namespace sese::system {

/// \brief 共享内存类
class SharedMemory final {
public:
    using Ptr = std::unique_ptr<SharedMemory>;

    /// 创建一块共享内存
    /// \param name 共享内存名称
    /// \param size 共享内存大小
    /// \return 共享内存对象指针
    /// \retval nullptr 创建失败
    static SharedMemory::Ptr create(const char *name, size_t size) noexcept;

    static Result<Ptr> createEx(const char *name, size_t size) noexcept;

    /// 使用一块现有的共享内存
    /// \param name 共享内存名称
    /// \return 共享内存对象指针
    /// \retval nullptr 获取失败
    static SharedMemory::Ptr use(const char *name) noexcept;

    static Result<Ptr> useEx(const char *name) noexcept;

    /// 析构函数
    ~SharedMemory() noexcept;
    /// 获取共享内存实际地址
    /// \return 共享内存块
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