#include "sese/net/http/RequestParser.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/internal/net/AsioIPConvert.h"
#include "sese/internal/net/http/AsioHttpClient.h"
#include "sese/util/Util.h"

using namespace sese::net::http;
using namespace sese::internal::net::http;

AsioHttpClient::AsioHttpClient()
    : ioContext(),
      socket(ioContext),
      sslContext(asio::ssl::context::tlsv12),
      sslSocket(socket, sslContext) {
}

AsioHttpClient::~AsioHttpClient() {
    socket.close();
}

int AsioHttpClient::getLastError() {
    return code.value();
}

std::string AsioHttpClient::getLastErrorString() {
    return code.message();
}

bool AsioHttpClient::init(const std::string &url, const std::string &proxy) {
    auto url_result = RequestParser::parse(url);
    req = std::move(url_result.request);
    if (!proxy.empty()) {
        auto proxy_result = RequestParser::parse(proxy);
        if (proxy_result.address == nullptr) {
            return false;
        }
        req->setUrl(url);
        req->set("via:", proxy);
        req->set("proxy-connection", "keep-alive");
        address = std::move(proxy_result.address);
        ssl = strcmpDoNotCase("https", proxy_result.url.getProtocol().c_str());
    } else {
        if (url_result.address == nullptr) {
            return false;
        }
        address = std::move(url_result.address);
        ssl = strcmpDoNotCase("https", url_result.url.getProtocol().c_str());
    }

    resp = std::make_unique<Response>();
    req->set("user-agent", "sese-httpclient/1.0");
    req->set("connection", "keep-alive");
    cookies = std::make_shared<CookieMap>();
    req->setCookies(cookies);

    return true;
}

bool AsioHttpClient::request() {
    // 允许重连一次
    int times = 0;
    if (first) {
        times += 1;
    } else {
        first = false;
    }
    while (!HttpUtil::sendRequest(this, req.get())) {
        if (times > 1) {
            return false;
        }

        times += 1;
        asio::ip::tcp::endpoint endpoint(convert(address), address->getPort());
        code = socket.connect(endpoint, code);
        if (code) {
            return false;
        }

        if (ssl) {
            code = sslSocket.handshake(asio::ssl::stream_base::client, code);
            if (code) {
                return false;
            }
        }
    }

    auto response_status = HttpUtil::recvResponse(this, resp.get());
    if (!response_status) {
        return false;
    }

    const auto CONNECTION_VALUE = resp->get("connection", "close");
    if (strcmpDoNotCase("close", CONNECTION_VALUE.c_str())) {
        first = true;
        if (ssl) {
            code = sslSocket.shutdown(code);
        }
        socket.close();
    }

    // 自动应用 cookie
    const auto DEST = req->getCookies();
    for (auto &&item: *resp->getCookies()) {
        DEST->add(item.second);
    }

    return true;
}

int64_t AsioHttpClient::read(void *buf, size_t len) {
    size_t read;
    if (ssl) {
        read = sslSocket.read_some(asio::buffer(buf, len), code);
    } else {
        read = socket.read_some(asio::buffer(buf, len), code);
    }

    if (code) {
        return -1;
    } else {
        return static_cast<int64_t>(read);
    }
}

int64_t AsioHttpClient::write(const void *buf, size_t len) {
    size_t wrote;
    if (ssl) {
        wrote = sslSocket.write_some(asio::buffer(buf, len), code);
    } else {
        wrote = socket.write_some(asio::buffer(buf, len), code);
    }

    if (code) {
        return -1;
    } else {
        return static_cast<int64_t>(wrote);
    }
}
