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

/// \file Process.h
/// \brief 进程类
/// \author kaoru
/// \version 0.1
/// \date 2023年3月13日

#pragma once

#include "sese/Util.h"
#include "sese/util/Result.h"

namespace sese::system {

/// 进程类
class Process {
public:
    using Ptr = std::unique_ptr<Process>;

    /// 创建一个新的进程对象
    /// \param command 命令
    /// \retval nullptr 创建失败
    static Process::Ptr create(const char *command) noexcept;


    static Result<Process::Ptr> createEx(const char *command) noexcept;

    /// 获取当前进程 ID
    /// \return 当前进程 ID
    [[nodiscard]] static pid_t getCurrentProcessId() noexcept;

    /// 等待进程结束
    /// \return 进程退出码
    [[nodiscard]] int wait() const noexcept;

    /// 结束进程
    /// \return 操作结果
    [[nodiscard]] bool kill() const noexcept;

    /// 获取进程 ID
    /// \return 获取进程对象 ID
    [[nodiscard]] pid_t getProcessId() const noexcept;

private:
    Process() = default;

#ifdef _WIN32
public:
    virtual ~Process() noexcept;

private:
    void *startup_info = nullptr;
    void *process_info = nullptr;
#else
public:
    virtual ~Process() noexcept = default;

private:
    // Unix-like 执行新进程实现
    static void exec(char *p_command) noexcept;
    // 计算字符串中参数的个数
    static size_t count(const char *p_command) noexcept;
    // 将下一个空格设置为 '\0'，并返回空格下一个字符的指针
    static char *spilt(char *p_command) noexcept;

    pid_t id = -1;
#endif
};
} // namespace sese::system