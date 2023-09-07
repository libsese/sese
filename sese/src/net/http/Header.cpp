#include <sese/net/http/Header.h>

#include <cctype>
#include <algorithm>

using namespace sese::net::http;

Header::Header(const std::initializer_list<KeyValueType> &initializerList) noexcept {
    for (const auto &item: initializerList) {
        headers.insert(item);
    }
}


#if defined (SESE_PLATFORM_WINDOWS) && !defined(__MINGW32__)
#define XX std::tolower
#else
#define XX ::tolower
#endif

Header *Header::set(const std::string &key, const std::string &value) noexcept {
    auto temp = key;
    std::transform(temp.begin(), temp.end(), temp.begin() , XX);
    headers[temp] = value;
    return this;
}

const std::string &Header::get(const std::string &key, const std::string &defaultValue) noexcept {
    auto temp = key;
    std::transform(temp.begin(), temp.end(), temp.begin() , XX);
    auto res = headers.find(temp);
    if (res == headers.end()) {
        return defaultValue;
    } else {
        return res->second;
    }
}

#undef XX

std::string_view Header::getView(const std::string &key, const std::string &defaultValue) noexcept {
    auto res = headers.find(key);
    if (res == headers.end()) {
        return defaultValue;
    } else {
        return res->second;
    }
}

const CookieMap::Ptr &Header::getCookies() const {
    return cookies;
}

void Header::setCookies(const CookieMap::Ptr &cookies) {
    Header::cookies = cookies;
}
