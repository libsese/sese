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
 * @file LogHelper.h
 * @author kaoru
 * @date April 17, 2022
 * @brief Log helper class
 */

#pragma once
#include <sese/record/Logger.h>
#include <sese/record/Event.h>

namespace sese::record {
/**
 * @brief Log helper class
 */
class  LogHelper {
public:
    using Ptr = std::shared_ptr<LogHelper>;

public:
    static void d(const char *format, ...) noexcept;
    static void i(const char *format, ...) noexcept;
    static void w(const char *format, ...) noexcept;
    static void e(const char *format, ...) noexcept;

public:
    explicit LogHelper();
    void debug(const char *format, ...);
    void info(const char *format, ...);
    void warn(const char *format, ...);
    void error(const char *format, ...);

private:
    static void l(Level level, const char *file, int32_t line, const char *format, va_list ap);
    void log(Level level, const char *file, int32_t line, const char *format, va_list ap);

    Logger *logger = nullptr;
};
} // namespace sese::record