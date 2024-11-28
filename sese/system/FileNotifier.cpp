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

#include "FileNotifier.h"

#include "sese/util/Util.h"

sese::Result<sese::system::FileNotifier::Ptr, sese::ErrorCode> sese::sys::FileNotifier::createEx(const std::string &path, FileNotifyOption *option) noexcept {
    if (auto result = create(path, option)) {
        return Result<Ptr, ErrorCode>::success(std::move(result));
    }
    return Result<Ptr, ErrorCode>::error({getErrorCode(), getErrorString()});
}
