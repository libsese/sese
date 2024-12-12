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
/// \brief Runtime Paths
/// \author kaoru
/// \date November 5, 2023

#pragma once

#include <sese/system/Path.h>
#include <sese/util/Initializer.h>

namespace sese::system {

/// Runtime path initialization task
class PathsInitiateTask : public InitiateTask {
public:
    PathsInitiateTask();

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/// Runtime Paths
class Paths {
public:
    static const Path &getWorkDir();

    static const Path &getExecutablePath();

    static const std::string &getExecutableName();
};
} // namespace sese::system
