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
 * @brief Exception class
 * @author kaoru
 * @date September 26, 2023
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/OutputStream.h>
#include <sese/log/Logger.h>
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
/// Unix exception wrapper class
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

/// Exception class
class Exception : public NativeException {
public:
    explicit Exception(std::string message);

    ~Exception() override;

    /// Print stack traces to stdout
    void printStacktrace();

    /// Print stack trace to logger
    /// \param logger Logger
    void printStacktrace(sese::log::Logger *logger);

    /// Print stack trace to output stream
    /// \param output Output stream
    void printStacktrace(sese::io::OutputStream *output);

    static uint16_t offset;

    /// @brief Get the text information of the exception
    /// @return Text information of the exception
    [[nodiscard]] const char *what() const noexcept override;

    /// @brief Get the text information of the exception
    /// @return Text information of the exception
    [[nodiscard]] std::string message() const noexcept;

protected:
    virtual std::string buildStacktrace();

    std::string *message_;
    system::StackInfo* stackInfo{};
};
} // namespace sese