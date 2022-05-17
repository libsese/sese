#include <sese/net/http/HttpUtil.h>

#ifndef _WIN32
#define _atoi64(val) strtoll(val, nullptr, 10)
#endif

using sese::http::HttpUtil;

bool HttpUtil::getLine(const Stream::Ptr &source, StringBuilder &builder) noexcept {
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
            // 剩下的 '\n'
            size = source->read(&ch, 1);
            if (size != 1) {
                return false;
            }
            break;
        }
    }
    return true;
}

bool HttpUtil::recvRequest(const Stream::Ptr &source, const RequestHeader::Ptr &request) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) {
        return false;
    }
    auto firstLines = builder.split(" ");
    builder.clear();

    // method
    if (0 == firstLines[0].compare("GET")) {
        request->setType(RequestType::Get);
    } else if (0 == firstLines[0].compare("POST")) {
        request->setType(RequestType::Post);
    } else {
        request->setType(RequestType::Another);
    }

    // url
    request->setUrl(firstLines[1]);

    // version
    if (0 == firstLines[2].compare("HTTP/1.1")) {
        request->setVersion(HttpVersion::VERSION_1_1);
    } else {
        request->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // keys & values
    if(!recvHeader(source, builder, request)) return false;

    return true;
}

bool sese::http::HttpUtil::sendRequest(const sese::Stream::Ptr &dest, const sese::http::RequestHeader::Ptr &request) noexcept {
    // method
    if (request->getType() == RequestType::Get) {
        if (-1 == dest->write("GET ", 4)) return false;
    } else if (request->getType() == RequestType::Post) {
        if (-1 == dest->write("POST ", 5)) return false;
    } else {
        return false;
    }

    // url
    if (-1 == dest->write(request->getUrl().c_str(), request->getUrl().length())) return false;

    // version
    if (request->getVersion() == HttpVersion::VERSION_1_1) {
        if (-1 == dest->write(" HTTP/1.1\r\n", 11)) return false;
    } else {
        return false;
    }

    // keys & values
    if (!sendHeader(dest, request)) return false;
    if (-1 == dest->write("\r\n", 2)) return false;

    return true;
}

bool HttpUtil::recvResponse(const Stream::Ptr &source, const ResponseHeader::Ptr &response) noexcept {
    StringBuilder builder;
    if (!getLine(source, builder)) return false;
    auto firstLines = builder.split(" ");
    builder.clear();

    // version
    if (0 == firstLines[0].compare("HTTP/1.1")) {
        response->setVersion(HttpVersion::VERSION_1_1);
    } else {
        response->setVersion(HttpVersion::VERSION_UNKNOWN);
    }

    // status code
    response->setCode((uint16_t)_atoi64(firstLines[1].c_str()));

    // keys & values
    if (!recvHeader(source, builder, response)) return false;

    return true;
}

bool HttpUtil::sendResponse(const sese::Stream::Ptr &dest, const sese::http::ResponseHeader::Ptr &response) noexcept {
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
    if (!sendHeader(dest, response)) return false;
    if (-1 == dest->write("\r\n", 2)) return false;

    return true;
}

bool HttpUtil::recvHeader(const Stream::Ptr &source, StringBuilder &builder, const Header::Ptr &header) noexcept {
    while (true) {
        if (!getLine(source, builder)) {
            return false;
        }

        // \r\n${无内容}\r\n
        if (builder.empty()) {
            break;
        } else {
            auto pair = builder.split(": ");
            builder.clear();
            header->set(pair[0], pair[1]);
        }
    }
    return true;
}

bool HttpUtil::sendHeader(const Stream::Ptr &dest, const Header::Ptr &header) noexcept {
    size_t len;
    for (auto pair: *header) {
        len = pair.first.length();
        if (-1 == dest->write(pair.first.c_str(), len)) return false;
        if (-1 == dest->write(": ", 2)) return false;
        len = pair.second.length();
        if (-1 == dest->write(pair.second.c_str(), len)) return false;
        if (-1 == dest->write("\r\n", 2)) return false;
    }

    if (-1 == dest->write("\r\n", 2)) return false;
    return true;
}