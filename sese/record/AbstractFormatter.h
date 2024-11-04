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
 * @file AbstractFormatter.h
 * @brief 日志格式化类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <string>
#include "sese/record/Event.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {
/**
 * @brief 日志格式化类
 */
class  AbstractFormatter {
public:
    typedef std::shared_ptr<AbstractFormatter> Ptr;

    virtual ~AbstractFormatter() noexcept = default;

    virtual std::string dump(const Event::Ptr &event) noexcept = 0;
};
} // namespace sese::record