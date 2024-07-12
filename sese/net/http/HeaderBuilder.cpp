#include <sese/net/http/HeaderBuilder.h>

using sese::net::http::Header;
using sese::net::http::HeaderBuilder;

HeaderBuilder::HeaderBuilder(Header &header) : header(header) {}

HeaderBuilder &&HeaderBuilder::set(const std::string &key, const std::string &value) && {
    header.set(key, value);
    return std::move(*this);
}
