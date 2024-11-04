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
 * @file DynamicTable.h
 * @author kaoru
 * @version 0.2
 * @brief HTTP 2 动态表
 * @date 2023年9月13日
 */

#pragma once

#include <deque>
#include <optional>
#include <string>

namespace sese::net::http {

/// HTTP2 动态表
class DynamicTable {
public:
    using Header = std::pair<std::string, std::string>;

    explicit DynamicTable(size_t max = 4096) noexcept;

    virtual ~DynamicTable() noexcept = default;

    void resize(size_t max) noexcept;

    [[nodiscard]] size_t getMaxSize() const noexcept { return max; }

    [[nodiscard]] size_t getCount() const noexcept { return queue.size(); }

    [[nodiscard]] size_t getSize() const noexcept { return size; }

    bool set(const std::string &key, const std::string &value) noexcept;

    [[nodiscard]] std::optional<Header> get(size_t index) const noexcept;

    [[nodiscard]] auto begin() const { return queue.begin(); }

    [[nodiscard]] auto end() const { return queue.end(); }

protected:
    size_t max;
    size_t size = 0;
    std::deque<Header> queue;
};
} // namespace sese::net::http