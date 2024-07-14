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

// GCOVR_EXCL_START

void CookieMap::expired(uint64_t now) {
    for (auto iterator = map.begin(); iterator != map.end();) {
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

void CookieMap::updateExpiresFrom(uint64_t offset) {
    for (auto iterator = map.begin(); iterator != map.end(); ++iterator) {
        iterator->second->updateExpiresFrom(offset);
    }
}

// GCOVR_EXCL_STOP