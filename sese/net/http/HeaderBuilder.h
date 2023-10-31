/// \file HeaderBuilder.h
/// \brief 头部键值集合构造器
/// \author kaoru
/// \date 2023年10月31日

#pragma once

#include <sese/net/http/Header.h>

namespace sese::net::http {

/// 头部键值集合构造器
class HeaderBuilder {
    Header &header;

public:
    explicit HeaderBuilder(Header &header);
    HeaderBuilder(HeaderBuilder &&other) noexcept ;
    HeaderBuilder &operator=(HeaderBuilder &&other) = delete;
    HeaderBuilder(const HeaderBuilder &other) = delete;
    const HeaderBuilder &operator=(const HeaderBuilder &) = delete;

    HeaderBuilder &&set(const std::string &key, const std::string &value) &&;
};

} // namespace sese::net::http