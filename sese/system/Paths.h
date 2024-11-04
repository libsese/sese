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

/// \file Paths.h
/// \brief 运行时路径
/// \author kaoru
/// \date 2023年11月5日

#pragma once

#include <sese/system/Path.h>
#include <sese/util/Initializer.h>

namespace sese::system {

/// 运行时路径初始化任务
class PathsInitiateTask : public InitiateTask {
public:
    PathsInitiateTask();

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/// 运行时路径
class Paths {
public:
    static const Path &getWorkDir();

    static const Path &getExecutablePath();

    static const std::string &getExecutableName();
};
} // namespace sese::system