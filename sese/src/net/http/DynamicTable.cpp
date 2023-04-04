#include "sese/net/http/DynamicTable.h"
#include "sese/net/http/HPACK.h"

sese::http::DynamicTable::DynamicTable(size_t max) noexcept {
    this->max = max;
}

void sese::http::DynamicTable::resize(size_t max) noexcept {
    while (size >= max) {
        decltype(auto) header = queue.back();
        size -= header.first.size() + header.second.size() + 4;
        queue.pop_back();
    }
    this->max = max;
}


bool sese::http::DynamicTable::set(const std::string &key, const std::string &value) noexcept {
    if (key.size() > std::numeric_limits<size_t>::max() - value.size()) {
        return false;
    }

    auto addition = key.size() + value.size() + 4;

    while (size + addition >= max) {
        decltype(auto) header = queue.back();
        size -= header.first.size() + header.second.size() + 4;
        queue.pop_back();
    }

    size += addition;
    queue.emplace_back(key, value);
    return true;
}

std::optional<sese::http::DynamicTable::Header> sese::http::DynamicTable::get(size_t index) const noexcept {
    auto s = predefined_headers.size() + queue.size();
    if (index < predefined_headers.size()) {
        return predefined_headers.at(index);
    } else if (index < predefined_headers.size() + queue.size()) {
        return queue.at(index - predefined_headers.size());
    }

    return std::nullopt;
}
