#include <sese/internal/service/http/HttpServiceImpl_V3.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/io/FakeStream.h>
#include <sese/Util.h>
#include <sese/record/Marco.h>

using namespace sese::internal::service::http::v3;

HttpServiceImpl::HttpServiceImpl()
    : io_context(),
      ssl_context(std::nullopt),
      acceptor(io_context) {
    thread = std::make_unique<Thread>(
            [this] {
                this->handeAccept();
                this->io_context.run();
            },
            "HttpServiveAcceptor"
    );
}

bool HttpServiceImpl::startup() {
    auto addr = net::convert(address);
    auto endpoint = asio::ip::tcp::endpoint(addr, address->getPort());

    if (HttpSerivce::ssl_context) {
        ssl_context = net::convert(HttpSerivce::ssl_context);
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
    conn->response.setCode(200);
    conn->response.set("server", this->serv_name);
}

void HttpServiceImpl::handeAccept() {
    auto conn = std::make_shared<HttpConnection>(shared_from_this(), io_context);
    acceptor.async_accept(
            conn->socket,
            [this, conn](const asio::error_code &error) {
                SESE_INFO("CONNECTED");
                if (HttpSerivce::ssl_context) {
                    conn->stream = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket &>>(conn->socket, ssl_context.value());
                    conn->stream->async_handshake(
                            asio::ssl::stream_base::server,
                            [conn](const asio::error_code &error) {
                                if (error) return;
                                SESE_INFO("HANDSHAKE");
                                HttpConnection::readHeader(conn);
                            }
                    );
                } else {
                    conn->readHeader(conn);
                    this->handeAccept();
                }
            }
    );
}

HttpConnection::HttpConnection(const std::shared_ptr<HttpServiceImpl> &service, asio::io_context &context)
    : stream(nullptr),
      socket(context),
      expect_length(0),
      real_length(0),
      service(service) {
}

void HttpConnection::readHeader(const HttpConnection::Ptr &conn) {
    if (conn->stream) {
        auto node = new iocp::IOBufNode(1024);
        conn->stream->async_read_some(asio::buffer(node->buffer, 1024), [conn, node](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                // 出现错误，应该断开连接
                delete node;
                return;
            }
            node->size = bytes_transferred;
            conn->io_buffer.push(node);
            bool recv_status = false;
            bool parse_status = false;
            for (int i = 0; i < bytes_transferred; ++i) {
                if (conn->is0x0d && (static_cast<char *>(node->buffer)[i] == '\n')) {
                    conn->is0x0d = false;
                    recv_status = true;
                    SESE_INFO("PARSING HEADER");
                    parse_status = sese::net::http::HttpUtil::recvRequest(&conn->io_buffer, &conn->request);
                    break;
                }
                conn->is0x0d = (static_cast<char *>(node->buffer)[i] == '\r');
            }
            if (!recv_status) {
                // 接收不完整，应该继续接收
                readHeader(conn);
                return;
            }
            if (!parse_status) {
                // 解析失败，应该断开连接
                return;
            }

            conn->expect_length = toInteger(conn->request.get("content-length", "0"));
            conn->real_length = node->getReadableSize();
            if (conn->real_length) {
                // 部分 body
                streamMove(&conn->request.getBody(), &conn->io_buffer, conn->real_length);
            }
            conn->io_buffer.clear();
            if (conn->expect_length != conn->real_length) {
                readBody(conn);
            } else {
                handleRequest(conn);
            }
        });
    } else {
        asio::async_read_until(conn->socket, conn->asio_dynamic_buffer, "\r\n", [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            auto std_input = std::istream{&conn->asio_dynamic_buffer};
            auto stream = io::StdInputStreamWrapper(std_input);
            streamMove(&conn->parse_buffer, &stream, bytes_transferred);
            if (bytes_transferred == 2) {
                SESE_INFO("PARSING HEADER");
                auto parse_status = sese::net::http::HttpUtil::recvRequest(&conn->parse_buffer, &conn->request);
                if (!parse_status) {
                    // 解析失败，应该断开连接
                    return;
                }
                conn->parse_buffer.freeCapacity();
                conn->expect_length = toInteger(conn->request.get("content-length", "0"));
                conn->real_length = 0;
                if (conn->expect_length != 0) {
                    readBody(conn);
                } else {
                    handleRequest(conn);
                }
            } else {
                readHeader(conn);
            }
        });
    }
}

void HttpConnection::readBody(const HttpConnection::Ptr &conn) {
    if (conn->stream) {
        auto node = new iocp::IOBufNode(1024);
        conn->stream->async_read_some(asio::buffer(node->buffer, 1024), [conn, node](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                // 出现错误，应该断开连接
                delete node;
                return;
            }
            node->size = bytes_transferred;
            conn->io_buffer.push(node);
            conn->real_length += node->size;
            streamMove(&conn->request.getBody(), &conn->io_buffer, node->size);
            if (conn->real_length >= conn->expect_length) {
                // 理论上 real_length 不可能大于 expect_length，此处预防万一
                conn->io_buffer.clear();
                handleRequest(conn);
            } else {
                readBody(conn);
            }
        });
    } else {
        asio::async_read(conn->socket, conn->asio_dynamic_buffer, [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            auto std_input = std::istream{&conn->asio_dynamic_buffer};
            auto stream = io::StdInputStreamWrapper(std_input);
            streamMove(&conn->request.getBody(), &stream, bytes_transferred);
            conn->real_length += bytes_transferred;
            if (conn->real_length >= conn->expect_length) {
                handleRequest(conn);
            } else {
                readBody(conn);
            }
        });
    }
}

void HttpConnection::handleRequest(const HttpConnection::Ptr &conn) {
    conn->service->handleRequest(conn);
    conn->response.set("content-length", std::to_string(conn->response.getBody().getLength()));

    if (conn->stream) {
        sese::net::http::HttpUtil::sendResponse(&conn->dynamic_buffer, &conn->response);
        conn->real_length = 0;
        conn->expect_length = conn->dynamic_buffer.getReadableSize();
        writeBody(conn);
    } else {
        auto std_output = std::ostream{&conn->asio_dynamic_buffer};
        auto stream = io::StdOutputStreamWrapper(std_output);
        sese::net::http::HttpUtil::sendResponse(&stream, &conn->response);
        conn->real_length = 0;
        conn->expect_length = conn->asio_dynamic_buffer.size();
        writeHeader(conn);
    }
}

void HttpConnection::writeHeader(const HttpConnection::Ptr &conn) {
    if (conn->stream) {
        auto size = conn->dynamic_buffer.peek(conn->send_buffer, MTU_VALUE);
        conn->stream->async_write_some(asio::buffer(conn->send_buffer, size), [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                // 出现错误，应该断开连接
                return;
            }
            conn->dynamic_buffer.trunc(bytes_transferred);
            conn->real_length += bytes_transferred;
            if (conn->real_length >= conn->expect_length) {
                conn->dynamic_buffer.freeCapacity();
                conn->expect_length = conn->response.getBody().getLength();
                conn->real_length = 0;

                if (conn->expect_length != 0) {
                    writeBody(conn);
                } else {
                    // todo: keepalive
                    SESE_INFO("CLOSE");
                }
            } else {
                writeHeader(conn);
            }
        });
    } else {
        asio::async_write(conn->socket, conn->asio_dynamic_buffer, [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            conn->real_length += bytes_transferred;
            if (conn->real_length >= conn->expect_length) {
                conn->expect_length = conn->response.getBody().getLength();
                conn->real_length = 0;

                if (conn->expect_length != 0) {
                    auto std_output = std::ostream{&conn->asio_dynamic_buffer};
                    auto stream = io::StdOutputStreamWrapper(std_output);
                    streamMove(&stream, &conn->response.getBody(), conn->expect_length);
                    writeBody(conn);
                } else {
                    // todo: keepalive
                    SESE_INFO("CLOSE");
                }
            } else {
                writeHeader(conn);
            }
        });
    }
}

void HttpConnection::writeBody(const HttpConnection::Ptr &conn) {
    if (conn->stream) {
        auto size = conn->dynamic_buffer.peek(conn->send_buffer, MTU_VALUE);
        conn->stream->async_write_some(asio::buffer(conn->send_buffer, size), [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                // 出现错误，应该断开连接
                return;
            }
            conn->dynamic_buffer.trunc(bytes_transferred);
            conn->real_length += bytes_transferred;
            if (conn->real_length >= conn->expect_length) {
                conn->dynamic_buffer.freeCapacity();
                // todo: keepalive
                SESE_INFO("CLOSE");
            } else {
                writeHeader(conn);
            }
        });
    } else {
        asio::async_write(conn->socket, conn->asio_dynamic_buffer, [conn](const asio::error_code &error, std::size_t bytes_transferred) {
            conn->real_length += bytes_transferred;
            if (conn->expect_length == conn->real_length) {
                conn->asio_dynamic_buffer.consume(conn->asio_dynamic_buffer.size());
                conn->expect_length = 0;
                conn->real_length = 0;
                // todo: keepalive
                SESE_INFO("CLOSE");
            } else {
                writeBody(conn);
            }
        });
    }
}
