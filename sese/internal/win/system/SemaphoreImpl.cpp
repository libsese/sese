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

#include <sese/system/Semaphore.h>

using sese::system::Semaphore;

class Semaphore::Impl {
public:
    using Ptr = std::unique_ptr<Impl>;

    HANDLE hSemaphore = nullptr;

    explicit Impl(HANDLE handle) : hSemaphore(handle) {}

    static Ptr create(std::string name, uint32_t initial_count) {
        auto handle = CreateSemaphoreA(
            nullptr,
            static_cast<LONG>(initial_count),
            static_cast<LONG>(initial_count),
            name.c_str()
        );
        if (!handle) {
            return nullptr;
        }

        return std::make_unique<Impl>(handle);
    }

    ~Impl() {
        CloseHandle(hSemaphore);
        hSemaphore = nullptr;
    }

    bool lock() {
        return WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, INFINITE);
    }

    // https://learn.microsoft.com/zh-cn/windows/win32/api/synchapi/nf-synchapi-releasesemaphore?source=docs
    bool unlock() {
        return 0 != ReleaseSemaphore(hSemaphore, 1, nullptr);
    }

    bool tryLock(std::chrono::milliseconds ms) {
        auto count = ms.count();
        return WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, static_cast<DWORD>(count));
    }
};

Semaphore::Ptr Semaphore::create(std::string name, uint32_t initial_count) {
    auto impl = Impl::create(name, initial_count);
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
