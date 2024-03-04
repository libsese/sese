// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/internal/service/AsioHttpService.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/util/Util.h>

sese::internal::service::AsioHttpSession::AsioHttpSession(
        asio::ip::tcp::socket &socket,
        asio::ssl::stream<asio::ip::tcp::socket &> *stream
)
    : socket(std::move(socket)), stream(stream) {
}

sese::internal::service::AsioHttpSession::~AsioHttpSession() {
    if (stream) { // NOLINT
        delete stream;
    }
}

sese::internal::service::AsioHttpService::AsioHttpService(
        sese::net::IPAddress::Ptr addr,
        security::SSLContext::Ptr ssl,
        size_t keep_alive,
        size_t max_buffer_size,
        std::function<void(sese::service::HttpSession *)> callback
)
    : io_context(),
      acceptor(io_context),
      addr(std::move(addr)),
      ssl(std::move(ssl)),
      keep_alive(keep_alive),
      MAX_BUFFER_SIZE(max_buffer_size),
      callback(std::move(callback)),
      thread([this] { io_context.run(); }, "AsioHttpService") {
}

sese::internal::service::AsioHttpService::~AsioHttpService() {
    if (ssl_context) { // NOLINT
        delete ssl_context;
    }
}

bool sese::internal::service::AsioHttpService::startup() {
    if (ssl) {
        ssl_context = new asio::ssl::context(std::move(net::convert(ssl)));
    }

    code = acceptor.open(asio::ip::tcp::v4(), code);
    if (code) {
        return false;
    }

    asio::ip::tcp::endpoint endpoint(net::convert(addr), addr->getPort());
    code = acceptor.bind(endpoint, code);
    if (code) {
        return false;
    }

    code = acceptor.listen(asio::socket_base::max_listen_connections, code);
    if (code) {
        return false;
    }

    acceptor.async_accept([this](asio::error_code error, asio::ip::tcp::socket socket) { onAsyncAccept(socket); });
    thread.start();

    return true;
}

bool sese::internal::service::AsioHttpService::shutdown() {
    io_context.stop();
    thread.join();
    return true;
}

int sese::internal::service::AsioHttpService::getLastError() {
    return static_cast<int>(code.value());
}

void sese::internal::service::AsioHttpService::onAsyncAccept(asio::ip::tcp::socket &client) {
    asio::error_code error;
    if (ssl) {
        auto stream = new asio::ssl::stream<asio::ip::tcp::socket &>(client, *ssl_context);
        error = stream->handshake(asio::ssl::stream_base::handshake_type::server, error);
        if (error) {
            error = client.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
            error = client.close(error);
            delete stream;
            return;
        }

        auto session = std::make_shared<AsioHttpSession>(client, stream);
        auto buffer = new iocp::IOBufNode(MAX_BUFFER_SIZE);
        session->recv_buf.push(buffer);
        sessionMap[client.native_handle()] = session;
        stream->async_read_some(asio::buffer(buffer->buffer, MAX_BUFFER_SIZE), [session, this, buffer](const asio::error_code &error, std::size_t bytes_transferred) {
            this->onAsyncRead(session, static_cast<char *>(buffer->buffer), bytes_transferred);
        });
    } else {
        auto session = std::make_shared<AsioHttpSession>(client, nullptr);
        auto buffer = new iocp::IOBufNode(MAX_BUFFER_SIZE);
        session->recv_buf.push(buffer);
        sessionMap[session->socket.native_handle()] = std::move(session);
        session->socket.async_read_some(asio::buffer(buffer->buffer, MAX_BUFFER_SIZE), [session, this, buffer](const asio::error_code &error, std::size_t bytes_transferred) {
            this->onAsyncRead(session, static_cast<char *>(buffer->buffer), bytes_transferred);
        });
    }
}

void sese::internal::service::AsioHttpService::onAsyncRead(const std::shared_ptr<AsioHttpSession> &session, const char *buffer, size_t bytes_transferred) {
    if (!session->recv_header) {
        size_t count = 0;
        for (int i = 0; i < bytes_transferred; ++i) {
            if (buffer[i] != '\r') {
                ++i;
                if (count == 0) {
                    session->recv_header = true;
                    break;
                }
            } else {
                count += 1;
            }
        }

        if (!session->recv_header) {
            if (ssl) {
                auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
                session->recv_buf.push(buf);
                session->stream->async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncRead(session, static_cast<char *>(buf->buffer), bytes_transferred);
                });
            } else {
                auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
                session->recv_buf.push(buf);
                session->socket.async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncRead(session, static_cast<char *>(buf->buffer), bytes_transferred);
                });
            }
            return;
        }
    }

    if (!session->parse_header) {
        // 此时可以开始解析 http 头部
        if (!sese::net::http::HttpUtil::recvRequest(&session->recv_buf, &session->req())) {
            asio::error_code error;
            error = session->socket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
            error = session->socket.close(error);
            return;
        }
        char *end;
        session->recv_body_size = std::strtol(session->req().get("content-length", "0").c_str(), &end, 10);
        session->parse_header = true;
    }

    if (!session->recv_body) {
        if (session->recv_body_size >= session->recv_buf.getReadableSize()) {
            if (ssl) {
                auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
                session->recv_buf.push(buf);
                session->stream->async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncRead(session, static_cast<char *>(buf->buffer), bytes_transferred);
                });
            } else {
                auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
                session->recv_buf.push(buf);
                session->socket.async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncRead(session, static_cast<char *>(buf->buffer), bytes_transferred);
                });
            }
            return;
        }
        streamMove(&session->req().getBody(), &session->recv_buf, session->recv_body_size);
        session->recv_body = true;
    }

    callback(session.get());

    // 此时开始响应请求
    if (!session->make_header) {
        auto length = session->resp().getBody().getReadableSize();
        session->resp().set("content-length", std::to_string(length));
        sese::net::http::HttpUtil::sendResponse(&session->send_buf, &session->resp());
        session->make_header = true;
    }

    session->buffer_length = session->send_buf.peek(session->buffer, MTU_VALUE);
    session->buffer_pos = 0;
    if (ssl) {
        session->stream->async_write_some(
                asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncWrite(session, bytes_transferred);
                }
        );
    } else {
        session->socket.async_write_some(
                asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                    this->onAsyncWrite(session, bytes_transferred);
                }
        );
    }
}

void sese::internal::service::AsioHttpService::onAsyncWrite(const std::shared_ptr<AsioHttpSession> &session, size_t bytes_transferred) {
    session->buffer_pos += bytes_transferred;
    if (!session->send_header) {
    resend_header:
        if (session->buffer_pos < session->buffer_length - 1) {
            if (ssl) {
                session->stream->async_write_some(
                        asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                        [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                            this->onAsyncWrite(session, bytes_transferred);
                        }
                );
            } else {
                session->socket.async_write_some(
                        asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                        [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                            this->onAsyncWrite(session, bytes_transferred);
                        }
                );
            }
        } else {
            session->buffer_length = session->send_buf.peek(session->buffer, MTU_VALUE);
            session->buffer_pos = 0;
            if (session->buffer_length != 0) {
                goto resend_header;
            }
        }
    }

    if (!session->send_body) {
    resend_body:
        if (session->buffer_pos < session->buffer_length - 1) {
            if (ssl) {
                session->stream->async_write_some(
                        asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                        [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                            this->onAsyncWrite(session, bytes_transferred);
                        }
                );
            } else {
                session->socket.async_write_some(
                        asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                        [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                            this->onAsyncWrite(session, bytes_transferred);
                        }
                );
            }
        } else {
            session->buffer_length = session->resp().getBody().peek(session->buffer, MTU_VALUE);
            session->buffer_pos = 0;
            if (session->buffer_length != 0) {
                goto resend_body;
            }
        }
    }

    // 完成一次响应，重置状态
    session->recv_buf.clear();
    session->recv_body_size = 0;
    session->recv_header = false;
    session->parse_header = false;
    session->recv_body = false;

    session->send_buf.freeCapacity();
    session->make_header = false;
    session->send_header = false;
    session->send_body = false;
    session->buffer_pos = 0;
    session->buffer_length = 0;
}