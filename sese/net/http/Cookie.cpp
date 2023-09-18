#include <sese/net/http/Cookie.h>
#include <ctime>

using namespace sese::net::http;

Cookie::Cookie(const std::string &name) noexcept {
    this->name = name;
}

Cookie::Cookie(const std::string &name, const std::string &value) noexcept {
    this->name = name;
    this->value = value;
}

bool Cookie::expired() const {
    time_t time;
    std::time(&time);
    return ((uint64_t)time > this->expires);
}

bool Cookie::isSecure() const {
    return secure;
}

void Cookie::setSecure(bool secure) {
    Cookie::secure = secure;
}

bool Cookie::isHttpOnly() const {
    return httpOnly;
}

void Cookie::setHttpOnly(bool httpOnly) {
    Cookie::httpOnly = httpOnly;
}

uint64_t  Cookie::getMaxAge() const {
    return maxAge;
}

void  Cookie::setMaxAge(uint64_t maxAge) {
    Cookie::maxAge = maxAge;
}

uint64_t  Cookie::getExpires() const {
    return expires;
}

void  Cookie::setExpires(uint64_t expires) {
    Cookie::expires = expires;
}

const std::string & Cookie::getName() const {
    return name;
}

const std::string & Cookie::getValue() const {
    return value;
}

void  Cookie::setValue(const std::string &value) {
    Cookie::value = value;
}

const std::string & Cookie::getDomain() const {
    return domain;
}

void  Cookie::setDomain(const std::string &domain) {
    Cookie::domain = domain;
}

const std::string & Cookie::getPath() const {
    return path;
}

void  Cookie::setPath(const std::string &path) {
    Cookie::path = path;
}
