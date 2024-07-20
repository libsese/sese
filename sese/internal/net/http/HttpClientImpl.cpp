#include "sese/net/http/HttpUtil.h"
#include "sese/internal/net/http/HttpClientImpl.h"
#include "sese/internal/net/AsioIPConvert.h"
#include "sese/io/ByteBuilder.h"
#include "sese/util/Util.h"

using namespace sese::net::http;
using namespace sese::internal::net::http;

HttpClientImpl::HttpClientImpl(const sese::net::IPAddress::Ptr &addr, sese::net::http::Request::Ptr req)
    : ioContext(),
      socket(ioContext) {
    address = addr;
    this->req = std::move(req);
    cookies = std::make_shared<CookieMap>();
    this->req->setCookies(cookies);
    resp = std::make_unique<Response>();
    reset();
}

HttpClientImpl::~HttpClientImpl() {
    socket.close();
}

// GCOVR_EXCL_START

int HttpClientImpl::getLastError() {
    return code.value();
}

std::string HttpClientImpl::getLastErrorString() {
    return code.message();
}

// GCOVR_EXCL_STOP

bool HttpClientImpl::request() {
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
    io::ByteBuilder bytes;
    HttpUtil::sendRequest(&bytes, req.get());
    while (!writeHeader(bytes)) {
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

        if (!handshake()) {
            reset();
            return false;
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
    if (req->getType() != RequestType::HEAD && expect > 0) {
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
        shutdown();
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

bool HttpClientImpl::handshake() {
    return true;
}

bool HttpClientImpl::shutdown() {
    return true;
}

bool HttpClientImpl::writeHeader(io::ByteBuilder &builder) {
    builder.resetPos();
    while (true) {
        char buffer[MTU_VALUE];
        auto len = builder.peek(buffer, MTU_VALUE);
        if (len == 0) {
            return true;
        }
        auto wrote = this->write(buffer, len);
        if (wrote <= 0) {
            return false;
        }
        builder.trunc(wrote);
    }
}

bool HttpClientImpl::writeBodyByCallback() {
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

bool HttpClientImpl::writeBodyByData() {
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

bool HttpClientImpl::writeBodyByAuto() {
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

bool HttpClientImpl::readBodyByCallback(size_t expect) {
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

bool HttpClientImpl::readBodyByData(size_t expect) {
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

int64_t HttpClientImpl::read(void *buf, size_t len) {
    size_t read = socket.read_some(asio::buffer(buf, len), code);
    if (code) {
        return -1;
    }
    return static_cast<int64_t>(read);
}

int64_t HttpClientImpl::write(const void *buf, size_t len) {
    size_t wrote = socket.write_some(asio::buffer(buf, len), code);
    if (code) {
        return -1;
    }
    return static_cast<int64_t>(wrote);
}
