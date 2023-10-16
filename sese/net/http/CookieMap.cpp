#include <sese/net/http/CookieMap.h>

using namespace sese::net::http;

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

void CookieMap::expired(uint64_t now) {
    for (decltype(auto) iterator = map.begin(); iterator != map.end();) {
        if (iterator->second->getExpires()) {
            if (iterator->second->expired(now)) {
                map.erase(iterator++);
            } else {
                iterator++;
            }
        } else {
            iterator++;
        }
    }
}

void CookieMap::update(uint64_t offset) {
    for (decltype(auto) iterator = map.begin(); iterator != map.end(); ++iterator) {
        iterator->second->update(offset);
    }
}