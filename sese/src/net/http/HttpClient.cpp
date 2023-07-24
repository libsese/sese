#include <sese/net/http/HttpClient.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Util.h>

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
            addr = parseAddress(result[0]);
            port = (uint16_t) std::strtol(result[1].c_str(), &endPtr, 10);
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
    if (ssl) {
        ptr->sslContext = security::SSLContextBuilder::SSL4Client();
    }

    return std::unique_ptr<HttpClient>(ptr);
}

bool HttpClient::doRequest() noexcept {
    if (!socket && !reconnect()) {
        // socket 未建立连接并且也无法连接
        return false;
    }

    if (isKeepAlive) {
        req.set("Connection", "Keep-Alive");
    }

    return sese::net::http::HttpUtil::sendRequest(socket.get(), &req);
}

bool HttpClient::doResponse() noexcept {
    if (socket) {
        auto rt = sese::net::http::HttpUtil::recvResponse(socket.get(), &resp);
        if (rt && isKeepAlive && sese::StrCmpI()(resp.get("Connection", "Close").c_str(), "Keep-Alive") == 0) {
            return rt;
        } else {
            socket->shutdown(Socket::ShutdownMode::Both);
            socket->close();
            socket = nullptr;
            return rt;
        }
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
    if (this->sslContext) {
        this->socket = this->sslContext->newSocketPtr(Socket::Family::IPv4, IPPROTO_IP);
    } else {
        this->socket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP, IPPROTO_IP);
    }

    if (0 != this->socket->connect(this->address)) {
        this->socket = nullptr;
        return false;
    } else {
        return true;
    }
}

int64_t HttpClient::read(void *buffer, size_t len) noexcept {
    if (socket) {
        return socket->read(buffer, len);
    }
    return -1;
}

int64_t HttpClient::write(const void *buffer, size_t len) noexcept {
    if (socket) {
        return socket->write(buffer, len);
    }
    return -1;
}