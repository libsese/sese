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
 * @file SimpleFormatter.h
 * @brief Simple formatter class
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/log/AbstractFormatter.h"
#include "sese/Config.h"
#include "sese/log/Event.h"

namespace sese::log {
/**
 * @brief Simple formatter class
 */
class SimpleFormatter final : public AbstractFormatter {
public:
    /**
     * Default constructor
     * @param text_pattern Format string, default is RECORD_DEFAULT_TEXT_PATTERN
     * @see RECORD_DEFAULT_TEXT_PATTERN
     * @verbatim
     * %    Escape symbol
     * li   Line number
     * lv   Log level
     * fn   File name
     * th   Thread ID
     * tn   Thread name
     * c    Complete time
     * m    Content
     * @endverbatim
     * @param time_pattern Time format string
     */
    explicit SimpleFormatter(const std::string &text_pattern = RECORD_DEFAULT_TEXT_PATTERN, const std::string &time_pattern = RECORD_DEFAULT_TIME_PATTERN) noexcept;

    std::string dump(const Event::Ptr &event) noexcept override;

    [[nodiscard]] const char *getTextPattern() const noexcept { return this->textPattern.c_str(); }

    [[nodiscard]] const char *getTimePattern() const noexcept { return this->timePattern.c_str(); }

private:
    std::string textPattern;
    std::string timePattern;
};
} // namespace sese::log