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

/// \file CommandLine.h
/// \brief Command line argument class
/// \author kaoru
/// \date October 26, 2023


#pragma once

#include <sese/Config.h>
#include <sese/util/NotInstantiable.h>
#include <sese/util/Initializer.h>

namespace sese::system {

/// Command line arguments initialize the task
class CommandLineInitiateTask final : public InitiateTask {
public:
    explicit CommandLineInitiateTask(int argc, const char *const *argv);

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/// Command line argument class
class CommandLine final : public NotInstantiable {
public:
    CommandLine() = delete;

    static int getArgc();

    static const char *const *getArgv();
};

} // namespace sese::system