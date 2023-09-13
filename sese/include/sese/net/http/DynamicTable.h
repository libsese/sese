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
}// namespace sese::http