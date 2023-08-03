#include <sese/net/http/HttpClient.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Util.h>
#include <sese/util/FixedBuilder.h>

using sese::net::IPv4Address;
using sese::net::IPv4AddressPool;
using sese::net::Socket;
using sese::net::http::HttpClient;
using sese::net::http::RequestHeader;
using sese::net::http::ResponseHeader;
using sese::net::http::Url;
using sese::text::StringBuilder;

HttpClient::Ptr HttpClient::create(const std::string &url, bool keepAlive) noexcept {
    auto info = Url(url);

    uint16_t port = 80;
    bool ssl = false;
    if (sese::StrCmpI()(info.getProtocol().c_str(), "Https") == 0) {
        port = 443;
        ssl = true;
    }

    IPv4Address::Ptr addr;
    if (info.getHost().find(':') == std::string::npos) {
        addr = parseAddress(info.getHost());
    } else {
        auto result = StringBuilder::split(info.getHost(), ":");
        if (result.size() == 2) {
            char *endPtr;
            port = (uint16_t) std::strtol(result[1].c_str(), &endPtr, 10);
            if (*endPtr != 0) {
                return nullptr;
            }
            addr = parseAddress(result[0]);
        } else {
            return nullptr;
        }
    }

    if (!addr) {
        return nullptr;
    }

    addr->setPort(port);

    auto ptr = new HttpClient;
    ptr->isKeepAlive = keepAlive;
    ptr->address = std::move(addr);
    ptr->req.setUrl(info.getUrl() + info.getQuery());
    ptr->req.set("host", info.getHost());
    if (ssl) {
        ptr->sslContext = security::SSLContextBuilder::SSL4Client();
    }

    return std::unique_ptr<HttpClient>(ptr);
}

void HttpClient::makeRequest() noexcept {
    buffer1.freeCapacity();

    if (isKeepAlive) {
        req.set("Connection", "Keep-Alive");
    }

    sese::net::http::HttpUtil::sendRequest(&buffer1, &req);
}

bool HttpClient::doRequest() noexcept {
    if (!isKeepAlive && !reconnect()) {
        // 非长连接并且无法重新建立连接
        return false;
    }

    if (!socket && !reconnect()) {
        // socket 未建立连接并且也无法连接
        return false;
    }

    char buf[MTU_VALUE];
    while (true) {
        auto len = buffer1.peek(buf, MTU_VALUE);
        if (len == 0) {
            return true;
        }
        auto l = socket->write(buf, len);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED || err == 0) {
                sese::sleep(0);
                continue;
            } else {
                // 断开连接...等
                return false;
            }
        } else {
            buffer1.trunc(l);
        }
    }
}

bool HttpClient::doResponse() noexcept {
    if (socket) {
        buffer2.freeCapacity();
        char buf[MTU_VALUE];
        while (true) {
            auto l = socket->read(buf, MTU_VALUE);
            if (l <= 0) {
                auto err = sese::net::getNetworkError();
                if (err == ENOTCONN) {
                    // 断开连接
                    return false;
                } else {
                    break;
                }
            } else {
                buffer2.write(buf, l);
            }
        }

        resp.clear();
        char *endPtr;
        auto rt = sese::net::http::HttpUtil::recvResponse(&buffer2, &resp);
        responseContentLength = std::strtol(resp.get("Content-Length", "0").c_str(), &endPtr, 10);
        return rt;
    }
    return false;
}

HttpClient::~HttpClient() noexcept {
    if (socket) {
        socket->close();
        socket = nullptr;
    }
}

IPv4Address::Ptr HttpClient::parseAddress(const std::string &host) noexcept {
    auto result = StringBuilder::split(host, ".");
    if (result.size() == 4) {
        for (decltype(auto) item: result) {
            char *endPtr;
            auto bit = std::strtol(item.c_str(), &endPtr, 10);
            if (*endPtr != 0) {
                goto lookup;
            }
            if (bit < 0 || bit > 255) {
                goto lookup;
            }
        }
        return IPv4Address::create(host.c_str(), 0);
    }

lookup:
    return IPv4AddressPool::lookup(host);
}

bool HttpClient::reconnect() noexcept {
    if (socket) {
        socket->close();
    }

    if (this->sslContext) {
        this->socket = this->sslContext->newSocketPtr(Socket::Family::IPv4, IPPROTO_IP);
    } else {
        this->socket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP, IPPROTO_IP);
    }

    this->socket->setNonblocking();

    if (0 != this->socket->connect(this->address)) {
        this->socket = nullptr;
        return false;
    } else {
        return true;
    }
}

int64_t HttpClient::read(void *buffer, size_t len) noexcept {
    if (socket) {
        return buffer2.read(buffer, len);
    }
    return -1;
}

int64_t HttpClient::write(const void *buffer, size_t len) noexcept {
    if (socket) {
        return buffer1.write(buffer, len);
    }
    return -1;
}