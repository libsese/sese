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
/// \brief Named Semaphore
/// \author kaoru
/// \date March 29, 2024

#ifndef SESE_CORE_SEMAPHORE_H
#define SESE_CORE_SEMAPHORE_H

#include <sese/Config.h>

#include <chrono>

#ifndef SESE_PLATFORM_WINDOWS
#include <semaphore.h>
#endif

namespace sese::system {

/// Named Semaphore
class Semaphore final {
public:
    using Ptr = std::unique_ptr<Semaphore>;

    /// Create a named semaphore
    /// \param name Name
    /// \param initial_count Initial count, default is 1 behaving like a mutex
    /// \retval nullptr Creation failed
    static Semaphore::Ptr create(std::string name, uint32_t initial_count = 1);

    ~Semaphore();

    /// Block and wait until resource is acquired
    bool lock();

    /// Release the current resource
    bool unlock();

    /// Try to acquire resource within a certain time
    /// \param ms Waiting time in milliseconds, invalid on APPLE platforms
    /// \return Whether acquisition is successful
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

