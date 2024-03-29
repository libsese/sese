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

/// \file Semaphore.h
/// \brief 命名信号量
/// \author kaoru
/// \date 2024年03月29日


#ifndef SESE_CORE_SEMAPHORE_H
#define SESE_CORE_SEMAPHORE_H


#include <sese/Config.h>

#include <chrono>


#ifndef SESE_PLATFORM_WINDOWS
#include <semaphore.h>
#endif

namespace sese::system {

/// 命名信号量
class Semaphore final {
public:
    using Ptr = std::unique_ptr<Semaphore>;

    /// 创建一个命名信号量
    /// \param name 名称
    /// \param initial_count 初始计数
    /// \param maximum_count 最大计数
    /// \retval nullptr 创建失败
    static Semaphore::Ptr create(std::string name, uint32_t initial_count);

    ~Semaphore();

    /// 阻塞并等待至获取到资源
    bool lock();

    /// 释放当前资源
    bool unlock();

    /// 在一定时间内尝试获取资源
    /// \param ms 单位为毫秒的等待时间
    /// \return 是否获取成功
    bool tryLock(std::chrono::milliseconds ms);

private:
    Semaphore() = default;

#ifdef SESE_PLATFORM_WINDOWS
    HANDLE hSemaphore{};
#else
    std::string sem_name{};
    sem_t *semaphore{};
#endif
};

}

#endif //SESE_CORE_SEMAPHORE_H
