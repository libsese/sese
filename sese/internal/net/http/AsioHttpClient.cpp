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

// GCOVR_EXCL_START

int AsioHttpClient::getLastError() {
    return code.value();
}

std::string AsioHttpClient::getLastErrorString() {
    return code.message();
}

// GCOVR_EXCL_STOP

bool AsioHttpClient::init(const std::string &url, const std::string &proxy) {
    auto url_result = RequestParser::parse(url);
    req = std::move(url_result.request);
    // 不方便CI测试
    // GCOVR_EXCL_START
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
    }
    // GCOVR_EXCL_STOP
    else {
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
    reset();
    return true;
}

bool AsioHttpClient::request() {
    // 处理内容长度
    char *end;
    if (expect_total) {
        req->set("content-length", std::to_string(expect_total));
    } else {
        req->set("content-length", std::to_string(req->getBody().getLength()));
    }

    // 允许重连一次
    int times = 0;
    if (first) {
        times += 1;
    } else {
        first = false;
    }
    while (!HttpUtil::sendRequest(this, req.get())) {
        if (times > 1) {
            reset();
            return false;
        }

        times += 1;
        asio::ip::tcp::endpoint endpoint(convert(address), address->getPort());
        code = socket.connect(endpoint, code);
        if (code) {
            reset();
            return false;
        }

        if (ssl) {
            code = sslSocket.handshake(asio::ssl::stream_base::client, code);
            if (code) {
                reset();
                return false;
            }
        }
    }

    // 判断如何读取 body
    if (expect_total && read_callback) {
        if (!writeBodyByCallback()) {
            reset();
            return false;
        }
    } else if (expect_total) {
        if (!writeBodyByData()) {
            reset();
            return false;
        }
    } else if (req->getBody().getLength()) {
        if (!writeBodyByAuto()) {
            reset();
            return false;
        }
    }

    auto response_status = HttpUtil::recvResponse(this, resp.get());
    if (!response_status) {
        reset();
        return false;
    }

    // 判断如何写入 body
    auto expect = std::strtol(resp->get("content-length", "0").c_str(), &end, 10);
    if (expect > 0) {
        if (write_callback && !readBodyByCallback(expect)) {
            reset();
            return false;
        }
        if (!readBodyByData(expect)) {
            reset();
            return false;
        }
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

    reset();
    return true;
}

bool AsioHttpClient::writeBodyByCallback() {
    size_t real = 0;
    while (true) {
        if (real >= expect_total) {
            return true;
        }
        auto need = std::min<size_t>(expect_total - real, MTU_VALUE);
        char buffer[MTU_VALUE];
        auto len = read_callback(buffer, need);
        if (len <= 0) {
            return false;
        }
        auto wrote = this->write(buffer, len);
        if (len != wrote) {
            return false;
        }
        real += wrote;
    }
}

bool AsioHttpClient::writeBodyByData() {
    size_t real = 0;
    while (true) {
        if (real >= expect_total) {
            return true;
        }
        auto need = std::min<size_t>(expect_total - real, MTU_VALUE);
        char buffer[MTU_VALUE];
        auto len = read_data->peek(buffer, need);
        auto wrote = this->write(buffer, len);
        if (wrote <= 0) {
            return false;
        }
        read_data->trunc(wrote);
        real += wrote;
    }
}

bool AsioHttpClient::writeBodyByAuto() {
    while (true) {
        char buffer[MTU_VALUE];
        auto len = req->getBody().peek(buffer, MTU_VALUE);
        if (len == 0) {
            return true;
        }
        auto wrote = this->write(buffer, len);
        if (wrote <= 0) {
            return false;
        }
        req->getBody().trunc(wrote);
    }
}

bool AsioHttpClient::readBodyByCallback(size_t expect) {
    size_t real = 0;
    while (true) {
        if (real >= expect) {
            return true;
        }
        char buffer[MTU_VALUE];
        auto need = std::min<size_t>(expect - real, MTU_VALUE);
        auto read = this->read(buffer, need);
        if (read <= 0) {
            return false;
        }
        if (read != write_callback(buffer, read)) {
            return false;
        }
        real += read;
    }
}

bool AsioHttpClient::readBodyByData(size_t expect) {
    size_t real = 0;
    while (true) {
        if (real >= expect) {
            return true;
        }
        char buffer[MTU_VALUE];
        auto need = std::min<size_t>(expect - real, MTU_VALUE);
        auto read = this->read(buffer, need);
        if (read <= 0) {
            return false;
        }
        if (read != write_data->write(buffer, read)) {
            return false;
        }
        real += read;
    }
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
    }
    return static_cast<int64_t>(read);
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
    }
    return static_cast<int64_t>(wrote);
}
