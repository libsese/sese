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

#include <semaphore.h>
#include <sese/system/Semaphore.h>

#ifdef SESE_PLATFORM_LINUX
#include <fcntl.h>
#include <sys/stat.h>
#endif

using sese::system::Semaphore;

class Semaphore::Impl {
public:
    using Ptr = std::unique_ptr<Impl>;

    sem_t *semaphore;
    std::string sem_name;

    Impl(sem_t *sem, std::string name) : semaphore(sem), sem_name(std::move(name)) {}

    static Ptr create(std::string name, uint32_t initial_count) {
        auto sem = sem_open(name.c_str(), O_CREAT | O_RDWR, 0666, initial_count);
        // System errors cannot be simulated
        // GCOVR_EXCL_START
        if (sem == SEM_FAILED) {
            return nullptr;
        }
        // GCOVR_EXCL_STOP

        return std::make_unique<Impl>(sem, std::move(name));
    }

    ~Impl() {
        sem_close(semaphore);
        sem_unlink(sem_name.c_str());
    }

    bool lock() {
        return 0 == sem_wait(semaphore);
    }

    bool unlock() {
        return 0 == sem_post(semaphore);
    }

    bool tryLock(std::chrono::milliseconds ms) {
#ifdef SESE_PLATFORM_LINUX
        struct timespec time {};
        time.tv_nsec = 1000000 * ms.count();
        return 0 == sem_timedwait(semaphore, &time);
#else
        return 0 == sem_trywait(semaphore);
#endif
    }
};

Semaphore::Ptr Semaphore::create(std::string name, uint32_t initial_count) {
    auto impl = Impl::create(std::move(name), initial_count);
    if (!impl) {
        return nullptr;
    }
    return MAKE_UNIQUE_PRIVATE(Semaphore, std::move(impl));
}

Semaphore::Semaphore(std::unique_ptr<Impl> impl) : impl(std::move(impl)) {}

Semaphore::~Semaphore() {}

bool Semaphore::lock() const {
    return impl->lock();
}

bool Semaphore::unlock() const {
    return impl->unlock();
}

bool Semaphore::tryLock(std::chrono::milliseconds ms) const {
    return impl->tryLock(ms);
}
