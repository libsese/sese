#include <sese/net/http/HttpClient.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/AddressPool.h>

using sese::IPv4AddressPool;
using sese::Socket;
using sese::http::HttpClient;
using sese::http::RequestHeader;
using sese::http::ResponseHeader;

HttpClient::Ptr HttpClient::create(const std::string &domain, uint16_t port) noexcept {
    auto address = IPv4AddressPool::lookup(domain);
    if (address) {
        address->setPort(port);
        auto clientSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP, 0);
        if (!clientSocket->setNonblocking(true)){
            return nullptr;
        }
        if (-1 != clientSocket->connect(address)) {
            auto client = std::unique_ptr<HttpClient>(new HttpClient);
            client->clientSocket = clientSocket;
            return client;
        }
    }
    return nullptr;
}

HttpClient::~HttpClient() noexcept {
    dispose();
}

bool HttpClient::request(RequestHeader *request) noexcept {
    if (clientSocket) {
        request->set("Client", HTTP_CLIENT_NAME)
                ->set("Connection", "Keep-Alive");
        return HttpUtil::sendRequest(clientSocket.get(), request);
    }
    return false;
}

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

ResponseHeader::Ptr HttpClient::response() noexcept {
    if (clientSocket) {
        auto header = std::make_unique<ResponseHeader>();
        if (HttpUtil::recvResponse(clientSocket.get(), header.get())) {
            auto str = header->get("Content-Length", "0");
            responseContentLength = std::stoll(str);
            isKeepAlive = (0 != strcasecmp(header->get("Keep-Alive", "Keep-Alive").c_str(), "Close"));
            return header;
        }
        dispose();
    }
    return nullptr;
}

int64_t HttpClient::read(void *buffer, size_t len) noexcept {
    if (clientSocket) {
        if (responseContentLength == 0) {
            return 0;
        }
        auto rt = clientSocket->read(buffer, len);
        responseContentLength -= rt;
        if (responseContentLength <= 0) {
            responseContentLength = 0;
            if (!isKeepAlive) {
                dispose();
            }
        }
        return rt;
    }
    return -1;
}

int64_t HttpClient::write(const void *buffer, size_t len) noexcept {
    if (clientSocket) {
        return clientSocket->write(buffer, len);
    }
    return -1;
}

bool HttpClient::check() noexcept {
    if (clientSocket) {
        return 0 == clientSocket->write(nullptr, 0);
    }
    return false;
}

void HttpClient::dispose() noexcept {
    if (clientSocket && isDetach) {
        clientSocket->shutdown(Socket::ShutdownMode::Both);
        clientSocket->close();
    }
}

socket_t HttpClient::detach() noexcept {
    isDetach = true;
    return clientSocket->getRawSocket();
}