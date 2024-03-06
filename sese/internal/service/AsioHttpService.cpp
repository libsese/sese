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


#include <sese/record/Marco.h>
#include <sese/internal/service/AsioHttpService.h>
#include <sese/internal/net/AsioSSLContextConvert.h>
#include <sese/internal/net/AsioIPConvert.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/util/Util.h>

#include <cassert>


using sese::internal::service::AsioHttpService;
using sese::internal::service::AsioHttpSession;

AsioHttpSession::AsioHttpSession(
        asio::ip::tcp::socket socket
) : socket(std::move(socket)), stream(nullptr) {
}

AsioHttpSession::~AsioHttpSession() {
    assert(true);
}

AsioHttpService::AsioHttpService(
        sese::net::IPAddress::Ptr addr,
        security::SSLContext::Ptr ssl,
        size_t keep_alive,
        size_t threads,
        size_t max_buffer_size,
        std::function<void(sese::service::HttpSession *)> callback
)
    : io_context(),
      acceptor(io_context),
      addr(std::move(addr)),
      ssl(std::move(ssl)),
      keep_alive(keep_alive),
      threads(),
      MAX_BUFFER_SIZE(max_buffer_size),
      callback(std::move(callback)) {
    for (size_t i = 0; i < threads; ++i) {
        auto th = Thread(
                [this] {
                    // 进行第一次异步请求
                    this->postAsyncAccept();
                    // 此时阻塞
                    this->io_context.run();
                },
                std::string("AsioHttpService_") + std::to_string(i)
        );
        this->threads.emplace_back(std::move(th));
    }
}

AsioHttpService::~AsioHttpService() {
    assert(true);
}

bool AsioHttpService::startup() {
    if (ssl) {
        ssl_context = new asio::ssl::context(net::convert(ssl));
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

    for (auto &&th: threads) {
        th.start();
    }
    return true;
}

bool AsioHttpService::shutdown() {
    io_context.stop();
    for (auto &&th: threads) {
        th.join();
    }
    return true;
}

int AsioHttpService::getLastError() {
    return static_cast<int>(code.value());
}

void AsioHttpService::onAsyncAccept(asio::ip::tcp::socket &client) {
    asio::error_code error;
    auto session = std::make_shared<AsioHttpSession>(std::move(client));
    if (ssl) {
        session->stream = new asio::ssl::stream<asio::ip::tcp::socket &>(session->socket, *ssl_context);
        error = session->stream->handshake(asio::ssl::stream_base::handshake_type::server, error);
        if (error) {
            error = client.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
            error = client.close(error);
            delete session->stream;
            session->stream = nullptr;
        } else {
            auto buffer = new iocp::IOBufNode(MAX_BUFFER_SIZE);
            session->recv_buf.push(buffer);
            postAsyncRead(session);
        }
    } else {
        auto buffer = new iocp::IOBufNode(MAX_BUFFER_SIZE);
        session->recv_buf.push(buffer);
        postAsyncRead(session);
    }

    // 在次提交监听新的连接
    this->postAsyncAccept();
}

void AsioHttpService::onAsyncRead(const std::shared_ptr<AsioHttpSession> &session, sese::iocp::IOBufNode *node, size_t bytes_transferred) {
    node->size += bytes_transferred;
    auto &&buffer = static_cast<char *>(node->buffer);

    if (!session->recv_header) {
        size_t count = 0;
        for (int i = 0; i < bytes_transferred - 1; ++i) {
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
            postAsyncRead(session);
            return;
        }
    }

    SESE_DEBUG("parsing");
    if (!session->parse_header) {
        // 此时可以开始解析 http 头部
        if (!sese::net::http::HttpUtil::recvRequest(&session->recv_buf, &session->req())) {
            asio::error_code error;
            error = session->socket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
            error = session->socket.close(error);
            SESE_DEBUG("failed to parsing");
            return;
        }
        char *end;
        session->recv_body_size = std::strtol(session->req().get("content-length", "0").c_str(), &end, 10);
        session->parse_header = true;

        session->keepalive = sese::strcmpDoNotCase(session->req().get("connect", "keep-alive").c_str(), "keep-alive");
    }
    SESE_DEBUG("parsed");

    if (!session->recv_body) {
        if (session->recv_body_size >= session->recv_buf.getReadableSize()) {
            postAsyncWrite(session);
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

    session->buffer_length = session->send_buf.read(session->buffer, MTU_VALUE);
    session->buffer_pos = 0;
    postAsyncWrite(session);
}

void AsioHttpService::onAsyncWrite(const std::shared_ptr<AsioHttpSession> &session, size_t bytes_transferred) {
    session->buffer_pos += bytes_transferred;

    if (!session->send_header) {
    resend_header:
        if (session->buffer_length != 0 && session->buffer_pos < session->buffer_length - 1) {
            postAsyncWrite(session);
            return;
        } else {
            session->buffer_length = static_cast<size_t>(session->send_buf.read(session->buffer, MTU_VALUE));
            session->buffer_pos = 0;
            if (session->buffer_length != 0) {
                goto resend_header;
            }
        }
        session->send_header = true;

        session->buffer_length = session->resp().getBody().read(session->buffer, MTU_VALUE);
        session->buffer_pos = 0;
    }

    if (!session->send_body) {
    resend_body:
        if (session->buffer_length != 0 && session->buffer_pos < session->buffer_length - 1) {
            postAsyncWrite(session);
        } else {
            session->buffer_length = static_cast<size_t>(session->resp().getBody().read(session->buffer, MTU_VALUE));

            session->buffer_pos = 0;
            if (session->buffer_length != 0) {
                goto resend_body;
            }
        }

        session->buffer_pos = 0;
        session->buffer_length = 0;
        session->send_body = true;
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

    session->req().clear();
    session->resp().clear();

    if (session->keepalive) {
        sese::sleep(0);
        postAsyncRead(session);
    } else {
        asio::error_code error;
        if (ssl) {
            error = session->stream->shutdown(error);
            delete session->stream;
            session->stream = nullptr;
        } else {
            error = session->socket.shutdown(asio::socket_base::shutdown_type::shutdown_both, error);
        }
        error = session->socket.close(error);
    }
}

void AsioHttpService::postAsyncAccept() {
    acceptor.async_accept([this](asio::error_code error, asio::ip::tcp::socket socket) { onAsyncAccept(socket); });
}

void AsioHttpService::postAsyncRead(const std::shared_ptr<AsioHttpSession> &session) {
    if (ssl) {
        auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
        session->recv_buf.push(buf);
        session->stream->async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                SESE_ERROR("%d %s", error.value(), error.message().c_str());
                asio::error_code e;
                if (ssl) {
                    e = session->stream->shutdown(e);
                } else {
                    e = session->socket.shutdown(asio::socket_base::shutdown_both, e);
                }
                e = session->socket.close(e);
                return;
            }
            this->onAsyncRead(session, buf, bytes_transferred);
        });
    } else {
        auto buf = new iocp::IOBufNode(MAX_BUFFER_SIZE);
        session->recv_buf.push(buf);
        session->socket.async_read_some(asio::buffer(buf->buffer, MAX_BUFFER_SIZE), [session, this, buf](const asio::error_code &error, std::size_t bytes_transferred) {
            if (error) {
                SESE_ERROR("%d %s", error.value(), error.message().c_str());
                asio::error_code e;
                if (ssl) {
                    e = session->stream->shutdown(e);
                } else {
                    e = session->socket.shutdown(asio::socket_base::shutdown_both, e);
                }
                e = session->socket.close(e);
                return;
            }
            this->onAsyncRead(session, buf, bytes_transferred);
        });
    }
}

void AsioHttpService::postAsyncWrite(const std::shared_ptr<AsioHttpSession> &session) {
    if (ssl) {
        session->stream->async_write_some(
                asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                    if (error) {
                        SESE_ERROR("%d %s", error.value(), error.message().c_str());
                        asio::error_code e;
                        if (ssl) {
                            e = session->stream->shutdown(e);
                        } else {
                            e = session->socket.shutdown(asio::socket_base::shutdown_both, e);
                        }
                        e = session->socket.close(e);
                        return;
                    }
                    this->onAsyncWrite(session, bytes_transferred);
                }
        );
    } else {
        session->socket.async_write_some(
                asio::buffer(session->buffer + session->buffer_pos, session->buffer_length - session->buffer_pos),
                [session, this](const asio::error_code &error, std::size_t bytes_transferred) {
                    if (error) {
                        SESE_ERROR("%d %s", error.value(), error.message().c_str());
                        asio::error_code e;
                        if (ssl) {
                            e = session->stream->shutdown(e);
                        } else {
                            e = session->socket.shutdown(asio::socket_base::shutdown_both, e);
                        }
                        e = session->socket.close(e);
                        return;
                    }
                    this->onAsyncWrite(session, bytes_transferred);
                }
        );
    }
}