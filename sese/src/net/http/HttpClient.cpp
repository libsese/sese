#include <sese/net/http/HttpClient.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Util.h>

#include <openssl/ssl.h>

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

HttpClient::Ptr sese::net::http::HttpClient::create(const IPv4Address::Ptr &address, bool ssl, bool keepAlive) noexcept {
    auto ptr = new HttpClient;
    ptr->isKeepAlive = keepAlive;
    ptr->address = address;
    if (ssl) {
        ptr->sslContext = security::SSLContextBuilder::SSL4Client();
    }
    return sese::net::http::HttpClient::Ptr(ptr);
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

    if (0 != this->socket->connect(this->address)) {
        this->socket = nullptr;
        return false;
    } else {
        SSL_set_mode((SSL *) std::dynamic_pointer_cast<security::SecuritySocket>(this->socket)->getSSL(), SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
        return true;
    }
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

    if (isKeepAlive) {
        req.set("connection", "keep-alive");
    }
    req.set("client", HTTP_CLIENT_NAME);

    req.set("content-length", std::to_string(req.getBody().getLength()));
    HttpUtil::sendRequest(&buffer, &req);

    // 发送头部
    while (true) {
        char buf[MTU_VALUE];
        auto len = buffer.peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = socket->write(buf, len);
        if (l > 0) {
            buffer.trunc(l);
        } else {
            return false;
        }
    }

    // 发送 body
    while (true) {
        char buf[MTU_VALUE];
        auto len = req.getBody().peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = socket->write(buf, len);
        if (l > 0) {
            req.getBody().trunc(l);
        } else {
            return false;
        }
    }

    req.getBody().freeCapacity();

    resp.clear();
    resp.getBody().freeCapacity();

    // 解析响应头部
    auto rt = HttpUtil::recvResponse(socket.get(), &resp);
    if (!rt) {
        return false;
    }

    char *end;
    auto len = std::strtol(resp.get("content-length", "0").c_str(), &end, 10);
    if (len == 0) {
        return true;
    }

    // 读取响应 body
    while (true) {
        char buf[MTU_VALUE];
        auto need = len >= MTU_VALUE ? MTU_VALUE : len;
        if (len == 0) {
            break;
        }
        auto l = socket->read(buf, need);
        if (l > 0) {
            resp.getBody().write(buf, l);
            len -= (int) l;
        } else {
            return false;
        }
    }
    return true;
}
