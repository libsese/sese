#include <sese/net/http/CookieMap.h>

using namespace sese::http;

void CookieMap::add(const Cookie::Ptr &cookie) noexcept {
    map[cookie->getName()] = cookie;
}

Cookie::Ptr CookieMap::find(const std::string &name) noexcept {
    auto iterator = map.find(name);
    if (iterator != map.end()) {
        return iterator->second;
    } else {
        return nullptr;
    }
}