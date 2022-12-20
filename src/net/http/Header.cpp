#include <sese/net/http/Header.h>

using sese::http::Header;

Header::Header(const std::initializer_list<KeyValueType> &initializerList) noexcept {
    for (const auto &item: initializerList) {
        headers.insert(item);
    }
}


Header *Header::set(const std::string &key, const std::string &value) noexcept {
    headers[key] = value;
    return this;
}

const std::string &Header::get(const std::string &key, const std::string &defaultValue) noexcept {
    auto res = headers.find(key);
    if (res == headers.end()) {
        return defaultValue;
    } else {
        return res->second;
    }
}

std::string_view Header::getView(const std::string &key, const std::string &defaultValue) noexcept {
    auto res = headers.find(key);
    if (res == headers.end()) {
        return defaultValue;
    } else {
        return res->second;
    }
}

const sese::http::CookieMap::Ptr &sese::http::Header::getCookies() const {
    return cookies;
}

void sese::http::Header::setCookies(const sese::http::CookieMap::Ptr &cookies) {
    Header::cookies = cookies;
}
