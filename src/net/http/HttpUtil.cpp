#include <sese/net/http/HttpUtil.h>
#include <sese/text/DateTimeFormatter.h>

#ifndef _WIN32
#define _atoi64(val) strtoll(val, nullptr, 10)
#endif

using sese::text::StringBuilder;
using sese::http::HttpUtil;

bool HttpUtil::getLine(Stream *source, StringBuilder &builder) noexcept {
    char ch;
    for (size_t i = 0; i < HTTP_MAX_SINGLE_LINE + 1; i++) {
        if (HTTP_MAX_SINGLE_LINE == i) return false;

        size_t size = source->read(&ch, 1);
        if (size != 1) {
            return false;
        }

        if (ch != '\r') {
            builder.append(ch);
        } else {
            // 剩下的 '\n'，读取的结果已经不重要
            source->read(&ch, 1);
            break;
        }
    }
    return true;
}

bool HttpUtil::recvRequest(Stream *source, RequestHeader *request) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) {
        return false;
    }
    auto firstLines = builder.split(" ");
    if (firstLines.size() != 3) return false;
    builder.clear();

    // method
    if ("GET" == firstLines[0]) {
        request->setType(RequestType::Get);
    } else if ("POST" == firstLines[0]) {
        request->setType(RequestType::Post);
    } else if ("OPTIONS" == firstLines[0]) {
        request->setType(RequestType::Options);
    } else if ("HEAD" == firstLines[0]) {
        request->setType(RequestType::Head);
    } else if ("PUT" == firstLines[0]) {
        request->setType(RequestType::Put);
    } else if ("DELETE" == firstLines[0]) {
        request->setType(RequestType::Delete);
    } else if ("TRACE" == firstLines[0]) {
        request->setType(RequestType::Trace);
    } else if ("CONNECT" == firstLines[0]) {
        request->setType(RequestType::Connect);
    } else {
        request->setType(RequestType::Another);
    }

    // uri
    request->setUrl(firstLines[1]);

    // version
    if ("HTTP/1.1" == firstLines[2]) {
        request->setVersion(HttpVersion::VERSION_1_1);
    } else {
        request->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // keys & values
    if (!recvHeader(source, builder, request, false)) return false;

    return true;
}

bool sese::http::HttpUtil::sendRequest(Stream *dest, RequestHeader *request) noexcept {
    // method
    switch (request->getType()) {
        case RequestType::Options:
            if (-1 == dest->write("OPTIONS ", 8)) return false;
            break;
        case RequestType::Get:
            if (-1 == dest->write("GET ", 4)) return false;
            break;
        case RequestType::Post:
            if (-1 == dest->write("POST ", 5)) return false;
            break;
        case RequestType::Head:
            if (-1 == dest->write("HEAD ", 5)) return false;
            break;
        case RequestType::Put:
            if (-1 == dest->write("PUT ", 4)) return false;
            break;
        case RequestType::Delete:
            if (-1 == dest->write("DELETE ", 7)) return false;
            break;
        case RequestType::Trace:
            if (-1 == dest->write("TRACE ", 6)) return false;
            break;
        case RequestType::Connect:
            if (-1 == dest->write("CONNECT ", 8)) return false;
            break;
        case RequestType::Another:
            break;
    }

    // uri
    if (-1 == dest->write(request->getUrl().c_str(), request->getUrl().length())) return false;

    // version
    if (request->getVersion() == HttpVersion::VERSION_1_1) {
        if (-1 == dest->write(" HTTP/1.1\r\n", 11)) return false;
    } else {
        return false;
    }

    // keys & values
    if (!sendHeader(dest, request, false)) return false;

    return true;
}

bool HttpUtil::recvResponse(Stream *source, ResponseHeader *response) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) return false;
    auto firstLines = builder.split(" ");
    builder.clear();

    // version
    if ("HTTP/1.1" == firstLines[0]) {
        response->setVersion(HttpVersion::VERSION_1_1);
    } else {
        response->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // status code
    response->setCode((uint16_t) _atoi64(firstLines[1].c_str()));

    // keys & values
    if (!recvHeader(source, builder, response, true)) return false;

    return true;
}

bool HttpUtil::sendResponse(Stream *dest, ResponseHeader *response) noexcept {
    // version
    if (response->getVersion() == HttpVersion::VERSION_1_1) {
        if (-1 == dest->write("HTTP/1.1 ", 9)) return false;
    } else {
        return false;
    }

    // status code
    std::string code = std::to_string(response->getCode());
    if (-1 == dest->write(code.c_str(), code.length())) return false;
    if (-1 == dest->write("\r\n", 2)) return false;

    // keys & values
    if (!sendHeader(dest, response, true)) return false;

    return true;
}

bool HttpUtil::recvHeader(Stream *source, StringBuilder &builder, Header *header, bool isResp) noexcept {
    CookieMap::Ptr cookies;
    if (isResp) {
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
            auto pair = builder.split(": ");
            if (pair.size() != 2) return false;
            builder.clear();
            if (isResp) {
                if (strcasecmp(pair[0].c_str(), "Set-Cookie") == 0) {
                    auto cookie = parseFromSetCookie(pair[1]);
                    if (cookie != nullptr) {
                        cookies->add(cookie);
                    }
                } else {
                    header->set(pair[0], pair[1]);
                }
            } else {
                if (strcasecmp(pair[0].c_str(), "Cookie") == 0) {
                    cookies = parseFromCookie(pair[1]);
                } else {
                    header->set(pair[0], pair[1]);
                }
            }
        }
    }
    header->setCookies(cookies);
    return true;
}

#define WRITE(buffer, size) \
    if (-1 == dest->write(buffer, size)) return false

bool HttpUtil::sendHeader(Stream *dest, Header *header, bool isResp) noexcept {
    size_t len;
    for (const auto &pair: *header) {
        len = pair.first.length();
        WRITE(pair.first.c_str(), len);
        WRITE(": ", 2);
        len = pair.second.length();
        WRITE(pair.second.c_str(), len);
        WRITE("\r\n", 2);
    }

    auto cookies = header->getCookies();
    if (cookies != nullptr) {
        if (isResp) {
            if (!sendSetCookie(dest, cookies)) return false;
        } else {
            if (!sendCookie(dest, cookies)) return false;
        }
    }


    if (-1 == dest->write("\r\n", 2)) return false;
    return true;
}

bool sese::http::HttpUtil::sendSetCookie(Stream *dest, const CookieMap::Ptr &cookies) noexcept {
    for (decltype(auto) cookie: *cookies) {
        if (-1 == dest->write("Set-Cookie: ", 12)) return false;
        const std::string &name = cookie.first;
        const std::string &value = cookie.second->getValue();
        WRITE(name.c_str(), name.size());
        WRITE("=", 1);
        WRITE(value.c_str(), value.size());

        const std::string &path = cookie.second->getPath();
        if (!path.empty()) {
            WRITE("; ", 2);
            WRITE(path.c_str(), path.size());
        }

        const std::string &domain = cookie.second->getDomain();
        if (!domain.empty()) {
            WRITE("; ", 2);
            WRITE(domain.c_str(), domain.size());
        }

        int64_t expires = cookie.second->getExpires();
        if (expires > 0) {
            WRITE("; Expires=", 10);
            auto date = DateTime(expires, 0);
            auto dateString = sese::text::DateTimeFormatter::format(date, TIME_GREENWICH_MEAN_PATTERN);
            WRITE(dateString.c_str(), dateString.size());
        }

        bool secure = cookie.second->isSecure();
        if (secure) {
            WRITE("; Secure", 8);
        }

        bool httpOnly = cookie.second->isHttpOnly();
        if (httpOnly) {
            WRITE("; HttpOnly", 10);
        }

        WRITE("\r\n", 2);
    }
    return true;
}

bool sese::http::HttpUtil::sendCookie(Stream *dest, const CookieMap::Ptr &cookies) noexcept {
    WRITE("Cookie: ", 8);

    bool isFirst = true;
    for (decltype(auto) cookie: *cookies) {
        if (isFirst) {
            isFirst = false;
        } else {
            WRITE("; ", 2);
        }
        const std::string &name = cookie.first;
        const std::string &value = cookie.second->getValue();
        WRITE(name.c_str(), name.size());
        WRITE("=", 1);
        WRITE(value.c_str(), value.size());
    }

    return true;
}

#undef WRITE

#define COMPARE(str1, str2) strcasecmp(str1.c_str(), str2) == 0

sese::http::Cookie::Ptr HttpUtil::parseFromSetCookie(const std::string &text) noexcept {
    auto result = StringBuilder::split(text, "; ");
    sese::http::Cookie::Ptr cookie = nullptr;
    bool isFirst = true;
    for (decltype(auto) one: result) {
        auto pair = StringBuilder::split(one, "=");
        if (isFirst) {
            if (pair.size() != 2) return nullptr;
            cookie = std::make_shared<sese::http::Cookie>(pair[0], pair[1]);
            isFirst = false;
        } else {
            if (pair.size() == 2) {
                // 键值对
                if (COMPARE(pair[0], "expires")) {
                    // todo 解析时间格式
                    // e.x. "Tue, 17 Oct 2023 15:41:22 GMT"

                } else if (COMPARE(pair[0], "path")) {
                    cookie->setPath(pair[0]);
                } else if (COMPARE(pair[0], "domain")) {
                    cookie->setDomain(pair[0]);
                }
            } else if (pair.size() == 1) {
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

sese::http::CookieMap::Ptr HttpUtil::parseFromCookie(const std::string &text) noexcept {
    auto cookies = std::make_shared<sese::http::CookieMap>();
    auto result = StringBuilder::split(text, "; ");
    for (decltype(auto) one: result) {
        auto pair = StringBuilder::split(one, "=");
        if (pair.size() != 2) continue;
        cookies->add(std::make_shared<sese::http::Cookie>(pair[0], pair[1]));
    }
    return cookies;
}