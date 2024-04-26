#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/util/Util.h>
#include <sese/record/Marco.h>

using namespace sese::internal::service::http::v3;

HttpServiceImpl::HttpServiceImpl()
    : io_context(),
      ssl_context(std::nullopt),
      acceptor(io_context) {
    thread = std::make_unique<Thread>(
            [this] {
                if (ssl_context.has_value()) {
                    this->handleSSLAccept();
                } else {
                    this->handleAccept();
                }
                this->io_context.run();
            },
            "HttpServiveAcceptor"
    );
}

bool HttpServiceImpl::startup() {
    auto addr = net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());

    if (HttpService::ssl_context) {
        ssl_context = net::convert(HttpService::ssl_context);
    }

    error = acceptor.open(
            addr.is_v4()
                    ? asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v4()
                    : asio::basic_socket_acceptor<asio::ip::tcp>::protocol_type::v6(),
            error
    );
    if (error) return false;

    error = acceptor.set_option(asio::socket_base::reuse_address(true), error);
    if (error) return false;

    error = acceptor.bind(endpoint, error);
    if (error) return false;

    error = acceptor.listen(asio::socket_base::max_listen_connections, error);
    if (error) return false;

    thread->start();

    return true;
}

bool HttpServiceImpl::shutdown() {
    error = acceptor.close(error);
    thread->join();
    return !error;
}

int HttpServiceImpl::getLastError() {
    return error.value();
}

void HttpServiceImpl::handleRequest(const HttpConnection::Ptr &conn) {
    auto iterator = controllers.find(conn->request.getUri());
    if (iterator == controllers.end()) {
        conn->response.setCode(404);
    } else {
        iterator->second(conn->request, conn->response);
    }

    auto keepalive_str = conn->request.get("connection", "close");
    conn->keepalive = strcmpDoNotCase(keepalive_str.c_str(), "keep-alive");

    if (conn->keepalive) {
        conn->response.set("connection", "keep-alive");
        conn->response.set("keep-alive", "timeout=" + std::to_string(keepalive));
    }

    conn->response.set("server", this->serv_name);
    conn->response.set("content-length", std::to_string(conn->response.getBody().getLength()));
}

void HttpServiceImpl::handleAccept() {
    auto conn = std::make_shared<HttpConnectionImpl>(shared_from_this(), io_context);
    acceptor.async_accept(
            conn->socket,
            [this, conn](const asio::error_code &error) {
                SESE_INFO("CONNECTED");
                conn->readHeader();
                this->handleAccept();
            }
    );
}

void HttpServiceImpl::handleSSLAccept() {
    auto conn = std::make_shared<HttpSSLConnectionImpl>(shared_from_this(), io_context);
    acceptor.async_accept(
            conn->socket,
            [this, conn](const asio::error_code &error) {
                SESE_INFO("CONNECTED");
                conn->stream = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket &>>(conn->socket, ssl_context.value());
                conn->stream->async_handshake(
                        asio::ssl::stream_base::server,
                        [conn](const asio::error_code &error) {
                            if (error) return;
                            SESE_INFO("HANDSHAKE");
                            conn->readHeader();
                        }
                );
                this->handleSSLAccept();
            }
    );
}

HttpConnection::HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : socket(context),
      timer(context),
      expect_length(0),
      real_length(0),
      service(service) {
}

void HttpConnection::reset() {
    request.clear();
    request.queryArgsClear();
    request.getBody().freeCapacity();
    if (auto cookies = request.getCookies()) cookies->clear();

    response.clear();
    response.getBody().freeCapacity();
    if (auto cookies = response.getCookies()) cookies->clear();
}
