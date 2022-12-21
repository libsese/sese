#include <sese/net/http/Cookie.h>
#include <ctime>

sese::http::Cookie::Cookie(const std::string &name) noexcept {
    this->name = name;
}

sese::http::Cookie::Cookie(const std::string &name, const std::string &value) noexcept {
    this->name = name;
    this->value = value;
}

bool sese::http::Cookie::expired() const {
    time_t time;
    std::time(&time);
    return time > this->expires;
}

bool sese::http::Cookie::isSecure() const {
    return secure;
}

void sese::http::Cookie::setSecure(bool secure) {
    Cookie::secure = secure;
}

bool sese::http::Cookie::isHttpOnly() const {
    return httpOnly;
}

void sese::http::Cookie::setHttpOnly(bool httpOnly) {
    Cookie::httpOnly = httpOnly;
}

uint64_t sese::http::Cookie::getExpires() const {
    return expires;
}

void sese::http::Cookie::setExpires(uint64_t expires) {
    Cookie::expires = expires;
}

const std::string &sese::http::Cookie::getName() const {
    return name;
}

const std::string &sese::http::Cookie::getValue() const {
    return value;
}

void sese::http::Cookie::setValue(const std::string &value) {
    Cookie::value = value;
}

const std::string &sese::http::Cookie::getDomain() const {
    return domain;
}

void sese::http::Cookie::setDomain(const std::string &domain) {
    Cookie::domain = domain;
}

const std::string &sese::http::Cookie::getPath() const {
    return path;
}

void sese::http::Cookie::setPath(const std::string &path) {
    Cookie::path = path;
}
