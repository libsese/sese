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

Semaphore::Ptr Semaphore::create(const char *name, uint32_t initial_count, uint32_t maximum_count) {
    auto handle = CreateSemaphoreA(
            nullptr,
            static_cast<LONG>(initial_count),
            static_cast<LONG>(maximum_count),
            name
    );
    if (!handle) {
        return nullptr;
    }

    auto res = new Semaphore();
    res->hSemaphore = handle;
    return std::unique_ptr<Semaphore>(res);
}

Semaphore::~Semaphore() {
    CloseHandle(hSemaphore);
    hSemaphore = nullptr;
}

void Semaphore::lock() {
    WaitForSingleObject(hSemaphore, INFINITE);
}

void Semaphore::unlock() {
    ReleaseSemaphore(hSemaphore, 1, nullptr);
}

bool Semaphore::tryLock(std::chrono::milliseconds ms) {
    auto count = ms.count();
    return WAIT_OBJECT_0 == WaitForSingleObject(hSemaphore, count);
}