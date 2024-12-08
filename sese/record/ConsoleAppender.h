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
 * @file ConsoleAppender.h
 * @author kaoru
 * @date March 28, 2022
 * @brief Console log appender class
 */

#pragma once

#include "sese/record/AbstractAppender.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"

namespace sese::record {
/**
 * @brief Console log appender class
 */
class  ConsoleAppender final : public AbstractAppender {
public:
    explicit ConsoleAppender(Level level = Level::DEBUG) noexcept;

    void dump(const char *buffer, size_t size) noexcept override;

public:
    static void setDebugColor() noexcept;
    static void setInfoColor() noexcept;
    static void setWarnColor() noexcept;
    static void setErrorColor() noexcept;
    static void setCleanColor() noexcept;
};
} // namespace sese::record