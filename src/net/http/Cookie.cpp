#include <sese/net/http/Cookie.h>
#include <ctime>

sese::http::Cookie::Cookie(const std::string &name) noexcept {
    this->name = name;
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

int32_t sese::http::Cookie::getVersion() const {
    return version;
}

void sese::http::Cookie::setVersion(int32_t version) {
    Cookie::version = version;
}

int64_t sese::http::Cookie::getExpires() const {
    return expires;
}

void sese::http::Cookie::setExpires(int64_t expires) {
    Cookie::expires = expires;
}

const std::string &sese::http::Cookie::getName() const {
    return name;
}

void sese::http::Cookie::setName(const std::string &name) {
    Cookie::name = name;
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

const std::string &sese::http::Cookie::getComment() const {
    return comment;
}

void sese::http::Cookie::setComment(const std::string &comment) {
    Cookie::comment = comment;
}

const std::string &sese::http::Cookie::getCommentUrl() const {
    return commentUrl;
}

void sese::http::Cookie::setCommentUrl(const std::string &commentUrl) {
    Cookie::commentUrl = commentUrl;
}
