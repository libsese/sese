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

/// @file ProcessBuilder.h
/// @brief Process Builder
/// @author kaoru
/// @date June 26, 2024

#pragma once

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>
#include <sese/system/Process.h>

namespace sese::system {

// GCOVR_EXCL_START

/// @brief Process Builder
class ProcessBuilder {
public:
    /**
     * @brief Initialize the process builder with the path of the executable file
     * @param exec Executable file path
     */
    explicit ProcessBuilder(const std::string &exec);

    /**
     * @brief Specify a single argument for the builder
     * @param arg Argument
     * @return ProcessBuilder&&
     */
    ProcessBuilder &&args(const std::string &arg) &&;

    /**
     * @brief Build the process
     * @see sese::system::Process::create()
     * @return Process::Ptr
     */
    Process::Ptr create();

    /**
     * @brief Build the process
     * @see sese::system::Process::createEx()
     * @return Result<Process::Ptr>
     */
    Result<Process::Ptr, ErrorCode> createEx();

private:
    text::StringBuilder builder;
};

// GCOVR_EXCL_STOP

} // namespace sese::system
