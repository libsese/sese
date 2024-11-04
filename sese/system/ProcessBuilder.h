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
/// @brief 进程构建器
/// @author kaoru
/// @date 2024年06月26日

#pragma once

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>
#include <sese/system/Process.h>

namespace sese::system {

// GCOVR_EXCL_START

/// @brief 进程构建器
class  ProcessBuilder {
public:
    /**
     * @brief 根据进程可执行文件路径初始化进程构建器
     * @param exec 可执行文件路径
     */
    explicit ProcessBuilder(const std::string &exec);

    /**
     * @brief 为构建器指定单个参数
     * @param arg 参数
     * @return ProcessBuilder&&
     */
    ProcessBuilder &&args(const std::string &arg) &&;

    /**
     * @brief 构建进程
     * @see sese::system::Process::create()
     * @return Process::Ptr
     */
    Process::Ptr create();

    /**
     * @brief 构建进程
     * @see sese::system::Process::createEx()
     * @return Result<Process::Ptr>
     */
    Result<Process::Ptr> createEx();

private:
    text::StringBuilder builder;
};

// GCOVR_EXCL_STOP

} // namespace sese::system