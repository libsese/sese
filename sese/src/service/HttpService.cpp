#include "sese/service/HttpService.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/UrlHelper.h"

#include "openssl/ssl.h"

using namespace sese;

void service::HttpConnection::doResponse() noexcept {
    net::http::HttpUtil::sendResponse(&buffer, &resp);
}

int64_t service::HttpConnection::read(void *buf, size_t len) {
    return this->buffer.read(buf, len);
}

int64_t service::HttpConnection::write(const void *buf, size_t len) {
    auto rt = this->buffer.write(buf, len);
    responseBodySize += rt;
    return rt;
}

service::HttpConfig::HttpConfig() noexcept {
    this->otherController = {
            [](HttpConnection *conn) {
                auto url = net::http::Url(conn->req.getUrl());

                // 忽略传输的内容
                char *end;
                auto len = std::strtol(conn->req.get("Content-Length", "0").data(), &end, 10);
                conn->buffer.trunc(len);

                // 404
                std::string content = "The controller or file named \"" + url.getUrl() + "\" could not be found.";
                conn->resp.setCode(404);
                conn->resp.set("Content-Length", std::to_string(content.length()));
                conn->doResponse();
                conn->write(content.c_str(), content.length());

                conn->status = HttpHandleStatus::OK;
            }};
}

service::HttpService::HttpService(const HttpConfig &config) noexcept {
    this->config = config;
}

void service::HttpService::onAccept(int fd) {
    SSL *clientSSL = nullptr;

    // 一般不会失败
    // GCOVR_EXCL_START
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
        return;
    }

    if (config.servCtx) {
        clientSSL = SSL_new((SSL_CTX *) config.servCtx->getContext());
        SSL_set_fd(clientSSL, (int) fd);
        SSL_set_accept_state(clientSSL);
        // GCOVR_EXCL_START
        while (true) {
            auto rt = SSL_do_handshake(clientSSL);
            if (rt <= 0) {
                // err is SSL_ERROR_WANT_READ or SSL_ERROR_WANT_WRITE
                auto err = SSL_get_error(clientSSL, rt);
                if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    SSL_free(clientSSL);
                    sese::net::Socket::close(fd);
                    return;
                }
            } else {
                break;
            }
        }
        // GCOVR_EXCL_STOP
    }

    auto conn = new HttpConnection();
    conn->ssl = clientSSL;
    conn->status = HttpHandleStatus::HANDING;

    this->createEvent(fd, EVENT_READ, conn);
}

void service::HttpService::onRead(event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;
    char buf[1024];
    while (true) {
        auto l = read(event->fd, buf, 1024, conn->ssl);
        if (l <= 0) {
            if (errno == ENOTCONN) {
                // 断开连接
                if (config.servCtx) {
                    SSL_free((SSL *) conn->ssl);
                }
                sese::net::Socket::close(event->fd);
                delete conn;
                this->freeEvent(event);
                break;
            } else {
                // 无数据可读，触发子函数处理
                onHandle(conn);
                if (conn->status == HttpHandleStatus::OK ||
                    conn->status == HttpHandleStatus::FILE) {
                    // 准备进入写模式，返回响应
                    event->events &= ~EVENT_READ;// 删除读事件
                    event->events |= EVENT_WRITE;// 添加写事件
                    this->setEvent(event);
                    break;
                } else {
                    // 处理错误，断开连接
                    if (config.servCtx) {
                        SSL_free((SSL *) conn->ssl);
                    }
                    sese::net::Socket::close(event->fd);
                    delete conn;
                    this->freeEvent(event);
                    break;
                }
            }
        } else {
            conn->buffer.write(buf, l);
            conn->requestSize += l;
        }
    }
}

void service::HttpService::onWrite(sese::event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;
    if (conn->status == HttpHandleStatus::OK) {
        bool finsh = false;
        char buf[1024]{};
        while (true) {
            auto len = conn->buffer.peek(buf, 1024);
            if (len == 0) {
                finsh = true;
                break;
            }
        retry:
            auto l = write(event->fd, buf, len, conn->ssl);
            if (l <= 0) {
                if (errno == ENOTCONN) {
                    // 断开连接
                    if (config.servCtx) {
                        SSL_free((SSL *) conn->ssl);
                    }
                    delete conn;// GCOVR_EXCL_LINE
                    sese::net::Socket::close(event->fd);
                    this->freeEvent(event);
                    break;
                } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // 不能继续写入，等待下一次触发可写事件
                    this->setEvent(event);
                    break;
                } else if (errno == EINTR) {
                    // 重试
                    goto retry;
                }
            } else {
                conn->buffer.trunc(l);
            }
        }
        if (finsh) {
            if (config.servCtx) {
                SSL_free((SSL *) conn->ssl);
            }
            delete conn;// GCOVR_EXCL_LINE
            sese::net::Socket::close(event->fd);
            this->freeEvent(event);
        }
    } else {
        // HttpHandleStatus::FILE
    }
}

void service::HttpService::onHandle(sese::service::HttpConnection *conn) noexcept {
    bool rt;
    conn->resp.set("Server", config.servName);

    rt = net::http::HttpUtil::recvRequest(&conn->buffer, &conn->req);
    if (!rt) {
        conn->status = HttpHandleStatus::FAIL;
        return;
    }

    auto url = net::http::Url(conn->req.getUrl());

    auto iterator1 = config.controller1Map.find(url.getUrl());
    if (iterator1 != config.controller1Map.end()) {
        rt = iterator1->second(conn->req,conn->resp);
        if (!rt) {
            conn->status = HttpHandleStatus::FAIL;
            return;
        }

        // 此处是向 buffer 中直接写入，故不需要判断是否成功
        net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
        conn->status = HttpHandleStatus::OK;
        return;
    }

    auto iterator2 = config.controller2Map.find(url.getUrl());
    if (iterator2 != config.controller2Map.end()) {
        iterator2->second(conn);
        return;
    }

    config.otherController(conn);
}

int64_t service::HttpService::read(int fd, void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t service::HttpService::write(int fd, const void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}