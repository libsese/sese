#include "sese/net/http/RequestParser.h"
#include "sese/internal/net/AsioIPConvert.h"
#include "sese/internal/net/http/AsioHttpClient.h"
#include "sese/util/Util.h"

using namespace sese::net::http;

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
    auto urlResult = RequestParser::parse(url);
    if (urlResult.address == nullptr) {
        return false;
    }

    req = std::move(urlResult.request);
    if (!proxy.empty()) {
        auto proxyResult = RequestParser::parse(proxy);
        if (proxyResult.address == nullptr) {
            return false;
        }
        req->setUrl(url);
        req->set("via:", proxy);
        req->set("proxy-connection", "keep-alive");
        address = std::move(proxyResult.address);
        ssl = sese::strcmpDoNotCase("https", proxyResult.url.getProtocol().c_str());
    } else {
        address = std::move(urlResult.address);
        ssl = sese::strcmpDoNotCase("https", urlResult.url.getProtocol().c_str());
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

    auto responseStatus = HttpUtil::recvResponse(this, resp.get());
    if (!responseStatus) {
        return false;
    }

    auto connectionValue = resp->get("connection", "close");
    if (sese::strcmpDoNotCase("close", connectionValue.c_str())) {
        first = true;
        if (ssl) {
            code = sslSocket.shutdown(code);
        }
        socket.close();
    }

    // 自动应用 cookie
    auto dest = req->getCookies();
    for (auto &&item: *resp->getCookies()) {
        dest->add(item.second);
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
