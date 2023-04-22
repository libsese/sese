#pragma once

#include <deque>
#include <optional>
#include <string>

namespace sese::net::http {

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

    protected:
        size_t max;
        size_t size = 0;
        std::deque<Header> queue;
    };
}// namespace sese::http