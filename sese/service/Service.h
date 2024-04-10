/// \file Service.h
/// \brief 服务接口
/// \author kaoru
/// \date 2024年03月4日

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

#pragma once

#include <memory>

namespace sese::service {

/// @brief 服务接口类
class Service {
public:
    using Ptr = std::unique_ptr<Service>;

    Service() = default;

    virtual ~Service() = default;

    virtual bool startup() = 0;

    virtual bool shutdown() = 0;

    virtual int getLastError() = 0;
};
} // namespace sese::service