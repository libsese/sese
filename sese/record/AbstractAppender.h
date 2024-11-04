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
 * @date 2022年3月28日
 * @brief 日志输出源基类
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
 * @brief 日志输出源基类
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
    /// 输出阈值
    Level level = Level::DEBUG;
};

} // namespace sese::record