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

#include <sese/system/Semaphore.h>

using sese::system::Semaphore;

Semaphore::Ptr Semaphore::create(std::string name, uint32_t initial_count) {
    auto handle = CreateSemaphoreA(
            nullptr,
            static_cast<LONG>(initial_count),
            static_cast<LONG>(initial_count),
            name.c_str()
    );
    if (!handle) {
        return nullptr;
    }

    auto res = MAKE_UNIQUE_PRIVATE(Semaphore);
    res->hSemaphore = handle;
    return res;
}

Semaphore::~Semaphore() {
    CloseHandle(hSemaphore);
    hSemaphore = nullptr;
}

bool Semaphore::lock() {
    return WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, INFINITE);
}

// https://learn.microsoft.com/zh-cn/windows/win32/api/synchapi/nf-synchapi-releasesemaphore?source=docs
bool Semaphore::unlock() {
    return 0 != ReleaseSemaphore(hSemaphore, 1, nullptr);
}

bool Semaphore::tryLock(std::chrono::milliseconds ms) {
    auto count = ms.count();
    return WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, static_cast<DWORD>(count));
}