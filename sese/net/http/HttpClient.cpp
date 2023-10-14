#include <sese/net/http/HttpClient.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/RequestParser.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Util.h>

#include <openssl/ssl.h>

using sese::net::IPv4Address;
using sese::net::Socket;
using sese::net::http::HttpClient;
using sese::net::http::RequestHeader;
using sese::net::http::ResponseHeader;
using sese::net::http::Url;

HttpClient::Ptr HttpClient::create(const std::string &url, bool keepAlive) noexcept {
    auto result = RequestParser::parse(url);

    bool ssl = false;
    if (sese::StrCmpI()(result.url.getProtocol().c_str(), "Https") == 0) {
        ssl = true;
    }

    if (!result.address) {
        return nullptr;
    }

    auto ptr = new HttpClient;
    ptr->isKeepAlive = keepAlive;
    ptr->req = std::move(result.request);
    ptr->resp = std::make_unique<Response>();
    if (result.address->getFamily() == AF_INET) {
        ptr->address = std::dynamic_pointer_cast<IPv4Address>(result.address);
    } else {
        return nullptr;
    }
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

bool HttpClient::reconnect() noexcept {
    if (socket) {
        socket->close();
    }

    if (this->sslContext) {
        this->socket = this->sslContext->newSocketPtr(Socket::Family::IPv4, IPPROTO_IP);

        if (0 != this->socket->connect(this->address)) {
            this->socket = nullptr;
            return false;
        } else {
            SSL_set_mode((SSL *) std::dynamic_pointer_cast<security::SecuritySocket>(this->socket)->getSSL(), SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
            return true;
        }
    } else {
        this->socket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP, IPPROTO_IP);

        if (0 != this->socket->connect(this->address)) {
            this->socket = nullptr;
            return false;
        } else {
            return true;
        }
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
        req->set("connection", "keep-alive");
    }
    req->set("client", HTTP_CLIENT_NAME);

    req->set("content-length", std::to_string(req->getBody().getLength()));
    HttpUtil::sendRequest(&buffer, req.get());

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
        auto len = req->getBody().peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = socket->write(buf, len);
        if (l > 0) {
            req->getBody().trunc(l);
        } else {
            return false;
        }
    }

    req->getBody().freeCapacity();

    resp->clear();
    resp->getBody().freeCapacity();

    // 解析响应头部
    auto rt = HttpUtil::recvResponse(socket.get(), resp.get());
    if (!rt) {
        return false;
    }

    char *end;
    auto len = std::strtol(resp->get("content-length", "0").c_str(), &end, 10);
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
            resp->getBody().write(buf, l);
            len -= (int) l;
        } else {
            return false;
        }
    }
    return true;
}
