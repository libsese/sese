#include <sese/net/http/HttpUtil.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/util/DateTime.h>

#ifndef _WIN32
#define _atoi64(val) strtoll(val, nullptr, 10) // NOLINT
#else
#pragma warning(disable : 4996)
#endif

using namespace sese::net::http;
using sese::text::StringBuilder;

bool HttpUtil::getLine(InputStream *source, StringBuilder &builder) noexcept {
    char ch;
    for (size_t i = 0; i < HTTP_MAX_SINGLE_LINE + 1; i++) {
        size_t size = source->read(&ch, 1);
        if (size != 1) {
            return false;
        }

        if (ch != '\r') {
            builder.append(ch);
        } else {
            // 剩下的 '\n'，读取的结果已经不重要
            source->read(&ch, 1);
            return true;
        }
    }
    return false;
}

bool HttpUtil::recvRequest(InputStream *source, RequestHeader *request) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) {
        return false;
    }
    auto first_lines = builder.split(" ");
    if (first_lines.size() != 3) return false;
    builder.clear();

    // method
    // GCOVR_EXCL_START
    if ("GET" == first_lines[0]) {
        request->setType(RequestType::GET);
    } else if ("POST" == first_lines[0]) {
        request->setType(RequestType::POST);
    } else if ("OPTIONS" == first_lines[0]) {
        request->setType(RequestType::OPTIONS);
    } else if ("HEAD" == first_lines[0]) {
        request->setType(RequestType::HEAD);
    } else if ("PUT" == first_lines[0]) {
        request->setType(RequestType::PUT);
    } else if ("DELETE" == first_lines[0]) {
        request->setType(RequestType::DELETE);
    } else if ("TRACE" == first_lines[0]) {
        request->setType(RequestType::TRACE);
    } else if ("CONNECT" == first_lines[0]) {
        request->setType(RequestType::CONNECT);
    } else {
        request->setType(RequestType::ANOTHER);
    }
    // GCOVR_EXCL_STOP

    // url
    request->setUrl(first_lines[1]);

    // version
    if ("HTTP/1.1" == first_lines[2]) {
        request->setVersion(HttpVersion::VERSION_1_1);
    } else {
        request->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // keys & values
    if (!recvHeader(source, builder, request, false)) return false;

    return true;
}

bool HttpUtil::sendRequest(OutputStream *dest, RequestHeader *request) noexcept {
    // method
    // GCOVR_EXCL_START
    switch (request->getType()) {
        case RequestType::OPTIONS:
            if (8 != dest->write("OPTIONS ", 8)) return false;
            break;
        case RequestType::GET:
            if (4 != dest->write("GET ", 4)) return false;
            break;
        case RequestType::POST:
            if (5 != dest->write("POST ", 5)) return false;
            break;
        case RequestType::HEAD:
            if (5 != dest->write("HEAD ", 5)) return false;
            break;
        case RequestType::PUT:
            if (4 != dest->write("PUT ", 4)) return false;
            break;
        case RequestType::DELETE:
            if (7 != dest->write("DELETE ", 7)) return false;
            break;
        case RequestType::TRACE:
            if (6 != dest->write("TRACE ", 6)) return false;
            break;
        case RequestType::CONNECT:
            if (8 != dest->write("CONNECT ", 8)) return false;
            break;
        case RequestType::ANOTHER:
            break;
    }
    // GCOVR_EXCL_STOP

    // url
    auto url = request->getUrl();
    if (url.length() != dest->write(url.c_str(), url.length())) return false; // GCOVR_EXCL_LINE

    // version
    // GCOVR_EXCL_START
    if (request->getVersion() == HttpVersion::VERSION_1_1) {
        if (11 != dest->write(" HTTP/1.1\r\n", 11)) return false;
    } else {
        return false;
    }
    // GCOVR_EXCL_STOP

    // keys & values
    if (!sendHeader(dest, request, false)) return false;

    return true;
}

bool HttpUtil::recvResponse(InputStream *source, ResponseHeader *response) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) return false;
    auto first_lines = builder.split(" ");
    builder.clear();

    // version
    if ("HTTP/1.1" == first_lines[0]) {
        response->setVersion(HttpVersion::VERSION_1_1);
    } else {
        response->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // status code
    response->setCode((uint16_t) _atoi64(first_lines[1].c_str()));

    // keys & values
    if (!recvHeader(source, builder, response, true)) return false;

    return true;
}

bool HttpUtil::sendResponse(OutputStream *dest, ResponseHeader *response) noexcept {
    // version
    if (response->getVersion() == HttpVersion::VERSION_1_1) {
        if (9 != dest->write("HTTP/1.1 ", 9)) return false;
    } else {
        return false;
    }

    // status code
    std::string code = std::to_string(response->getCode());
    if (code.length() != dest->write(code.c_str(), code.length())) return false; // GCOVR_EXCL_LINE
    if (2 != dest->write("\r\n", 2)) return false;                               // GCOVR_EXCL_LINE

    // keys & values
    if (!sendHeader(dest, response, true)) return false;

    return true;
}

bool HttpUtil::recvHeader(InputStream *source, StringBuilder &builder, Header *header, bool is_resp) noexcept {
    CookieMap::Ptr cookies;
    if (is_resp) {
        cookies = std::make_shared<CookieMap>();
    }

    while (true) {
        if (!getLine(source, builder)) {
            return false;
        }

        // \r\n${无内容}\r\n
        if (builder.empty()) {
            break;
        } else {
            // auto pair = builder.split(": ");
            // if (pair.size() != 2) return false;
            auto line = builder.toString();
            builder.clear();
            auto pos = line.find_first_of(": ");
            if (pos == std::string::npos) {
                return false;
            }
            auto key = line.substr(0, pos);
            auto value = line.substr(pos + 2);
            if (is_resp) {
                if (strcasecmp(key.c_str(), "Set-Cookie") == 0) {
                    auto cookie = parseFromSetCookie(value);
                    if (cookie != nullptr) {
                        cookies->add(cookie);
                    }
                } else {
                    header->set(key, value);
                }
            } else {
                if (strcasecmp(key.c_str(), "Cookie") == 0) {
                    cookies = parseFromCookie(value);
                } else {
                    header->set(key, value);
                }
            }
        }
    }
    header->setCookies(cookies);
    return true;
}

#define WRITE(buffer, size) \
    if (size != dest->write(buffer, size)) return false

bool HttpUtil::sendHeader(OutputStream *dest, Header *header, bool is_resp) noexcept {
    size_t len;
    for (const auto &pair: *header) {
        len = pair.first.length();
        WRITE(pair.first.c_str(), len); // GCOVR_EXCL_LINE
        WRITE(": ", 2);                 // GCOVR_EXCL_LINE
        len = pair.second.length();
        WRITE(pair.second.c_str(), len); // GCOVR_EXCL_LINE
        WRITE("\r\n", 2);                // GCOVR_EXCL_LINE
    }

    auto cookies = header->getCookies();
    if (cookies != nullptr) {
        if (is_resp) {
            if (!sendSetCookie(dest, cookies)) return false;
        } else {
            if (!sendCookie(dest, cookies)) return false;
        }
    }


    if (-1 == dest->write("\r\n", 2)) return false; // GCOVR_EXCL_LINE
    return true;
}

bool HttpUtil::sendSetCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept {
    for (decltype(auto) cookie: *cookies) {
        WRITE("Set-Cookie: ", 12); // GCOVR_EXCL_LINE
        const std::string &name = cookie.first;
        const std::string &value = cookie.second->getValue();
        WRITE(name.c_str(), name.size());   // GCOVR_EXCL_LINE
        WRITE("=", 1);                      // GCOVR_EXCL_LINE
        WRITE(value.c_str(), value.size()); // GCOVR_EXCL_LINE

        const std::string &path = cookie.second->getPath();
        if (!path.empty()) {
            WRITE("; Path=", 7);              // GCOVR_EXCL_LINE
            WRITE(path.c_str(), path.size()); // GCOVR_EXCL_LINE
        }

        const std::string &domain = cookie.second->getDomain();
        if (!domain.empty()) {
            WRITE("; Domain=", 9);                // GCOVR_EXCL_LINE
            WRITE(domain.c_str(), domain.size()); // GCOVR_EXCL_LINE
        }

        uint64_t max_age = cookie.second->getMaxAge();
        if (max_age > 0) {
            WRITE("; Max-Age=", 10); // GCOVR_EXCL_LINE
            auto age = std::to_string(max_age);
            WRITE(age.c_str(), age.length()); // GCOVR_EXCL_LINE
        } else {
            uint64_t expires = cookie.second->getExpires();
            if (expires > 0) {
                WRITE("; Expires=", 10); // GCOVR_EXCL_LINE
                auto date = DateTime(expires * 1000000, 0);
                auto date_string = sese::text::DateTimeFormatter::format(date, TIME_GREENWICH_MEAN_PATTERN);
                WRITE(date_string.c_str(), date_string.size()); // GCOVR_EXCL_LINE
            }
        }

        bool secure = cookie.second->isSecure();
        if (secure) {
            WRITE("; Secure", 8); // GCOVR_EXCL_LINE
        }

        bool http_only = cookie.second->isHttpOnly();
        if (http_only) {
            WRITE("; HttpOnly", 10); // GCOVR_EXCL_LINE
        }

        WRITE("\r\n", 2); // GCOVR_EXCL_LINE
    }
    return true;
}

bool HttpUtil::sendCookie(OutputStream *dest, const CookieMap::Ptr &cookies) noexcept {
    if (!cookies->empty()) {
        WRITE("Cookie: ", 8); // GCOVR_EXCL_LINE
    }

    bool is_first = true;
    for (decltype(auto) cookie: *cookies) {
        if (is_first) {
            is_first = false;
        } else {
            WRITE("; ", 2); // GCOVR_EXCL_LINE
        }
        const std::string &name = cookie.first;
        const std::string &value = cookie.second->getValue();
        WRITE(name.c_str(), name.size());   // GCOVR_EXCL_LINE
        WRITE("=", 1);                      // GCOVR_EXCL_LINE
        WRITE(value.c_str(), value.size()); // GCOVR_EXCL_LINE
    }

    if (!cookies->empty()) {
        WRITE("\r\n", 2); // GCOVR_EXCL_LINE;
    }

    return true;
}

#undef WRITE

#define COMPARE(str1, str2) strcasecmp(str1.c_str(), str2) == 0

Cookie::Ptr HttpUtil::parseFromSetCookie(const std::string &text) noexcept {
    auto result = StringBuilder::split(text, "; ");
    Cookie::Ptr cookie = nullptr;
    bool is_first = true;
    for (decltype(auto) one: result) {
        auto index = one.find('=');
        if (is_first) {
            if (index == std::string::npos) return nullptr;
            cookie = std::make_shared<Cookie>(one.substr(0, index), one.substr(index + 1));
            is_first = false;
        } else {
            if (index != std::string::npos) {
                // 键值对
                if (COMPARE(one.substr(0, index), "max-age")) {
                    cookie->setMaxAge(std::stoll(one.substr(index + 1)));
                } else if (COMPARE(one.substr(0, index), "expires")) {
                    cookie->setExpires(text::DateTimeFormatter::parseFromGreenwich(one.substr(index + 1)));
                } else if (COMPARE(one.substr(0, index), "path")) {
                    cookie->setPath(one.substr(index + 1));
                } else if (COMPARE(one.substr(0, index), "domain")) {
                    cookie->setDomain(one.substr(index + 1));
                }
            } else {
                // 属性
                if (COMPARE(one, "secure")) {
                    cookie->setSecure(true);
                } else if (COMPARE(one, "httponly")) {
                    cookie->setHttpOnly(true);
                }
            }
        }
    }

    return cookie;
}

#undef COMPARE

CookieMap::Ptr HttpUtil::parseFromCookie(const std::string &text) noexcept {
    auto cookies = std::make_shared<CookieMap>();
    auto result = StringBuilder::split(text, "; ");
    for (decltype(auto) one: result) {
        auto pair = StringBuilder::split(one, "=");
        if (pair.size() != 2) continue;
        cookies->add(std::make_shared<Cookie>(pair[0], pair[1]));
    }
    return cookies;
}