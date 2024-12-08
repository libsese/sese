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
 * @file AbstractAppender.h
 * @author kaoru
 * @date March 28, 2022
 * @brief Base class for log appenders
 */

#pragma once

#include "sese/Config.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {

/**
 * @brief Base class for log appenders
 */
class  AbstractAppender {
public:
    typedef std::shared_ptr<AbstractAppender> Ptr;

    AbstractAppender() noexcept = default;

    virtual ~AbstractAppender() noexcept = default;

    explicit AbstractAppender(Level level) noexcept;

    virtual void dump(const char *buffer, size_t size) noexcept = 0;

    [[nodiscard]] Level getLevel() const noexcept { return level; }

    void setLevel(Level lv) noexcept { this->level = lv; }

protected:
    /// Output threshold
    Level level = Level::DEBUG;
};

} // namespace sese::record