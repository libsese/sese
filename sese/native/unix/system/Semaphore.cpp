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

//
// Created by kaoru on 2024-03-29.
//

#include <semaphore.h>
#include <sese/system/Semaphore.h>

#ifdef SESE_PLATFORM_LINUX
#include <fcntl.h>
#include <sys/stat.h>
#endif

using sese::system::Semaphore;

Semaphore::Ptr Semaphore::create(std::string name, uint32_t initial_count) {
    auto sem = sem_open(name.c_str(), O_CREAT | O_RDWR, 0666, initial_count);
    // 系统错误无法模拟
    // GCOVR_EXCL_START
    if (sem == SEM_FAILED) {
        return nullptr;
    }
    // GCOVR_EXCL_STOP

    auto res = new Semaphore();
    res->semaphore = sem;
    res->sem_name = std::move(name);
    return std::unique_ptr<Semaphore>(res);
}

Semaphore::~Semaphore() {
    sem_close(semaphore);
    sem_unlink(sem_name.c_str());
}

bool Semaphore::lock() {
    return 0 == sem_wait(semaphore);
}

bool Semaphore::unlock() {
    return 0 == sem_post(semaphore);
}

bool Semaphore::tryLock(std::chrono::milliseconds ms) {
#ifdef SESE_PLATFORM_LINUX
    struct timespec time {};
    time.tv_nsec = 1000000 * ms.count();
    return 0 == sem_timedwait(semaphore, &time);
#else
    return 0 == sem_trywait(semaphore);
#endif
}