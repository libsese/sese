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

#include "sese/plugin/ClassFactory.h"

sese::plugin::BaseClass::Ptr sese::plugin::ClassFactory::createClassWithName(const std::string &name) noexcept {
    auto iterator = infoMap.find(name);
    if (iterator != infoMap.end()) {
        return iterator->second.builder();
    }
    return nullptr;
}

sese::Result<sese::plugin::BaseClass::Ptr, sese::ErrorCode> sese::plugin::ClassFactory::createClassWithNameEx(const std::string &name) noexcept {
    if (auto result = createClassWithName(name)) {
        return Result<BaseClass::Ptr, ErrorCode>::success(std::move(result));
    }
    return Result<BaseClass::Ptr, ErrorCode>::error({1, "Cannot find class named " + name});
}

const sese::plugin::ClassFactory::RegisterInfoMapType &sese::plugin::ClassFactory::getRegisterClassInfo() noexcept {
    return infoMap;
}
