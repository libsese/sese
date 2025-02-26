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

#include <sese/log/Logger.h>
#include <sese/system/SharedMemory.h>
#include <sese/util/Initializer.h>

#include <chrono>
#include <memory>
#include <thread>
#include <random>

using namespace std::chrono_literals;
using sese::log::Logger;

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto mem = sese::system::SharedMemory::create("SharedMemoryForSese", sizeof(int64_t));
    if (mem == nullptr) {
        Logger::error("failed to create shared memory: errno {}", errno);
        return -1;
    } else {
        Logger::info("create success");
    }

    auto random = static_cast<int64_t>(std::random_device()());
    Logger::info("select random number: {}", random);
    auto buffer = mem->getBuffer();
    auto p_int = static_cast<int64_t *>(buffer);
    *p_int = random;

    while (true) {
        if (*p_int == random + 1) {
            break;
        } else {
            std::this_thread::sleep_for(500ms);
        }
    }

    Logger::info("recv change number: {}", *p_int);

    return 0;
}