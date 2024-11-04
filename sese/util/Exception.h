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

/**
 * @file Exception.h
 * @brief 异常类
 * @author kaoru
 * @date 2023年9月26日
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/OutputStream.h>
#include <sese/record/Logger.h>
#include <sese/system/StackInfo.h>

#include <exception>
#include <utility>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4275)
#endif

namespace sese {

#if defined(_MSC_VER)
using NativeException = std::exception;
#else
/// Unix 异常包装类
class UnixException : public std::exception {
public:
    explicit UnixException(const char *message) : std::exception(), msg(message) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return msg;
    }

private:
    const char *msg{};
};

using NativeException = UnixException;
#endif

/// 异常类
class Exception : public NativeException {
public:
    explicit Exception(std::string message);

    ~Exception() override;

    /// 向标准输出打印堆栈跟踪
    void printStacktrace();

    /// 向日志器打印堆栈跟踪
    /// \param logger 日志器
    void printStacktrace(sese::record::Logger *logger);

    /// 向输出流打印堆栈跟踪
    /// \param output 输出流
    void printStacktrace(sese::io::OutputStream *output);

    static uint16_t offset;

    /// @brief 获取异常的文本信息
    /// @return 异常的文本信息
    [[nodiscard]] const char *what() const noexcept override;

    /// @brief 获取异常的文本信息
    /// @return 异常的文本信息
    [[nodiscard]] std::string message() const noexcept;

protected:
    virtual std::string buildStacktrace();

    std::string *message_;
    system::StackInfo* stackInfo{};
};
} // namespace sese