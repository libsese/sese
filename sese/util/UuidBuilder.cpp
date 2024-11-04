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

#include "sese/util/UuidBuilder.h"

#include <thread>

sese::UuidBuilder::UuidBuilder() noexcept
    : timestampHandler(std::chrono::system_clock::now()) {
}

// 按照 SimpleUuid 提供的接口处理时钟回拨问题，此处逻辑不需要在此论证
// GCOVR_EXCL_START
bool sese::UuidBuilder::generate(sese::Uuid &dest) noexcept {
    auto timestamp = timestampHandler.tryGetCurrentTimestamp();
    if (timestamp == 0) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timestamp = timestampHandler.tryGetCurrentTimestamp();
            if (timestamp == 0) {
                continue;
            } else if (timestamp == UINT64_MAX) {
                return false;
            } else {
                dest.setTimestamp(timestamp);
                return true;
            }
        }
        return false;
    } else if (timestamp == UINT64_MAX) {
        return false;
    } else {
        dest.setTimestamp(timestamp);
        return true;
    }
}
// GCOVR_EXCL_STOP