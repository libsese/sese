#include "sese/service/HttpService.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/UrlHelper.h"
#include "sese/util/Util.h"

#include "openssl/ssl.h"

#include <filesystem>

using namespace sese;

service::HttpConnection::~HttpConnection() noexcept {
    if (file) {
        file->close();
        file = nullptr;
    }
}

void service::HttpConnection::doResponse() noexcept {
    net::http::HttpUtil::sendResponse(&buffer2, &resp);
}

int64_t service::HttpConnection::read(void *buf, size_t len) {
    return this->buffer1.read(buf, len);
}

int64_t service::HttpConnection::write(const void *buf, size_t len) {
    auto rt = this->buffer2.write(buf, len);
    responseBodySize += rt;
    return rt;
}

service::HttpConfig::HttpConfig() noexcept {
    this->otherController = {
            [](HttpConnection *conn) {
                auto url = net::http::Url(conn->req.getUrl());

                // 忽略传输的内容
                // char *end;
                // auto len = std::strtol(conn->req.get("Content-Length", "0").data(), &end, 10);
                // conn->buffer1.trunc(len);

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
    conn->fd = fd;
    conn->ssl = clientSSL;
    conn->status = HttpHandleStatus::HANDING;

    if (config.keepalive) {
        conn->timeoutEvent = TimerableService::createTimeoutEvent(fd, conn, config.keepalive);
        conn->timeoutEvent->data = conn;
    }

    conn->event = this->createEvent(fd, EVENT_READ, conn);
}

void service::HttpService::onRead(event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;

    if (conn->keepalive) {
        TimerableService::cancelTimeoutEvent(conn->timeoutEvent);
    }

    char buf[1024];
    while (true) {
        auto l = read(event->fd, buf, 1024, conn->ssl);
        if (l <= 0) {
            if (errno == ENOTCONN) {
                // 断开连接
                goto free;
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
                    goto free;
                }
            }
        } else {
            conn->buffer1.write(buf, l);
            conn->requestSize += l;
        }
    }

    return;
free:
    if (conn->keepalive) {
        TimerableService::freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEvent(event);
}

void service::HttpService::onWrite(sese::event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;
    // 普通控制器处理
    if (conn->status == HttpHandleStatus::OK) {
        onControllerWrite(event);
    }
    // 文件处理
    else if (conn->status == HttpHandleStatus::FILE) {
        onFileWrite(event);
    }
}

void service::HttpService::onHandle(sese::service::HttpConnection *conn) noexcept {
    bool rt;
    conn->resp.set("Server", config.servName);

    rt = net::http::HttpUtil::recvRequest(&conn->buffer1, &conn->req);
    if (!rt) {
        conn->status = HttpHandleStatus::FAIL;
        return;
    }

    if (config.keepalive) {
        conn->keepalive = sese::StrCmpI()(conn->req.get("Connection", "close").c_str(), "keep-alive") == 0;
        conn->resp.set("Connection", "keep-alive");
        conn->resp.set("Keep-Alive", "timeout=" + std::to_string(config.keepalive) + " ,max = 0");
    }

    auto url = net::http::Url(conn->req.getUrl());

    auto iterator1 = config.controller1Map.find(url.getUrl());
    if (iterator1 != config.controller1Map.end()) {
        rt = iterator1->second(conn->req, conn->resp);
        if (!rt) {
            conn->status = HttpHandleStatus::FAIL;
            return;
        }

        // 此处是向 buffer 中直接写入，故不需要判断是否成功
        net::http::HttpUtil::sendResponse(&conn->buffer2, &conn->resp);
        conn->status = HttpHandleStatus::OK;
        return;
    }

    auto iterator2 = config.controller2Map.find(url.getUrl());
    if (iterator2 != config.controller2Map.end()) {
        iterator2->second(conn);
        return;
    }

    if (!config.workDir.empty()) {
        auto file = config.workDir + url.getUrl();
        if (std::filesystem::is_regular_file(file)) {
            conn->file = FileStream::create(file, "rb");
            if (conn->file != nullptr) {
                auto len = std::filesystem::file_size(file);
                conn->responseBodySize = len;
                conn->resp.set("Content-Length", std::to_string(len));
                conn->doResponse();
                conn->status = HttpHandleStatus::FILE;
                return;
            }
        }
    }

    config.otherController(conn);
}

void service::HttpService::onControllerWrite(event::BaseEvent *event) noexcept {
    auto conn = (HttpConnection *) event->data;
    char buf[1024]{};
    while (true) {
        auto len = conn->buffer2.peek(buf, 1024);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                // 不能继续写入，等待下一次触发可写事件
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->buffer2.trunc(l);
        }
    }
    if (conn->keepalive) {
        this->setTimeoutEvent(conn->timeoutEvent, config.keepalive);
        // 进入下一次请求
        conn->event->events &= ~EVENT_WRITE;
        conn->event->events |= EVENT_READ;
        this->setEvent(conn->event);
        return;
    } else {
        goto free;
    }

free:
    if (conn->keepalive) {
        this->freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEvent(event);
}

void service::HttpService::onFileWrite(event::BaseEvent *event) noexcept {
    auto conn = (HttpConnection *) event->data;
    char buf[1024]{};
    // 先发送头部
    while (true) {
        auto len = conn->buffer2.peek(buf, 1024);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                // 不能继续写入，等待下一次触发可写事件
                this->setEvent(event);
                break;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->buffer2.trunc(l);
        }
    }
    // 发送内容
    while (true) {
        auto need = conn->responseBodySize - conn->responseBodyWroteSize;
        need = need >= 1024 ? 1024 : need % 1024;
        if (need == 0) {
            break;
        }
        auto len = conn->file->peek(buf, need);
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
                // 不能继续写入，等待下一次触发可写事件
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->file->trunc(l);
            conn->responseBodyWroteSize += l;
        }
    }
    if (conn->responseBodySize == conn->responseBodyWroteSize) {
        if (conn->keepalive) {
            conn->responseBodySize = 0;
            conn->responseBodyWroteSize = 0;
            this->setTimeoutEvent(conn->timeoutEvent, config.keepalive);
            // 进入下一次请求
            conn->event->events &= ~EVENT_WRITE;
            conn->event->events |= EVENT_READ;
            this->setEvent(conn->event);
            return;
        } else {
            goto free;
        }
    }

free:
    if (conn->keepalive) {
        this->freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEvent(event);
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

void service::HttpService::onTimeout(service::TimeoutEvent *timeoutEvent) {
    auto conn = (HttpConnection *) timeoutEvent->data;
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(conn->fd);
    this->freeEvent(conn->event);
    delete conn;
}