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

#include "sese/util/TimestampHandler.h"

sese::TimestampHandler::TimestampHandler(std::chrono::system_clock::time_point point) noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(point.time_since_epoch());
}

uint64_t sese::TimestampHandler::getCurrentTimestamp() noexcept {
    latest = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    return latest.count();
}

uint64_t sese::TimestampHandler::tryGetCurrentTimestamp() noexcept {
    auto new_point = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if (new_point > latest) {
        latest = new_point;
        return latest.count();
    } else {
        if (latest - new_point > std::chrono::seconds(5)) {
            return UINT64_MAX;
        } else {
            return 0;
        }
    }
}