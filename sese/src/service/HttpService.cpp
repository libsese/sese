#include "sese/service/HttpService.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/UrlHelper.h"

#include "openssl/ssl.h"

using namespace sese;

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

    this->buffers[fd] = new sese::ByteBuilder(2048);
    this->createEvent(fd, EVENT_READ, clientSSL);
}

void service::HttpService::onRead(event::BaseEvent *event) {
    auto buffer = this->buffers[event->fd];
    char buf[1024];
    while (true) {
        auto l = read(event->fd, buf, 1024, event->data);
        if (l <= 0) {
            if (errno == ENOTCONN) {
                // 断开连接
                if (config.servCtx) {
                    SSL_free((SSL *) event->data);
                }
                buffers.erase(event->fd);
                delete buffer;// GCOVR_EXCL_LINE
                sese::net::Socket::close(event->fd);
                this->freeEvent(event);
                break;
            } else {
                // 无数据可读，触发子函数处理
                if (onHandle(buffer)) {
                    // 准备进入写模式，返回响应
                    event->events &= ~EVENT_READ;// 删除读事件
                    event->events |= EVENT_WRITE;// 添加写事件
                    this->setEvent(event);
                    break;
                } else {
                    // 处理错误，断开连接
                    buffers.erase(event->fd);
                    if (config.servCtx) {
                        SSL_free((SSL *) event->data);
                    }
                    delete buffer;// GCOVR_EXCL_LINE
                    sese::net::Socket::close(event->fd);
                    this->freeEvent(event);
                    break;
                }
            }
        } else {
            buffer->write(buf, l);
        }
    }
}

void service::HttpService::onWrite(sese::event::BaseEvent *event) {
    bool finsh = false;
    auto buffer = buffers[event->fd];
    char buf[1024];
    while (true) {
        auto len = buffer->peek(buf, 1024);
        if (len == 0) {
            finsh = true;
            break;
        }
    retry:
        auto l = write(event->fd, buf, len, event->data);
        if (l <= 0) {
            if (errno == ENOTCONN) {
                // 断开连接
                if (config.servCtx) {
                    SSL_free((SSL *) event->data);
                }
                buffers.erase(event->fd);
                delete buffer;// GCOVR_EXCL_LINE
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
            buffer->trunc(l);
        }
    }

    if (finsh) {
        if (config.servCtx) {
            SSL_free((SSL *) event->data);
        }
        buffers.erase(event->fd);
        delete buffer;// GCOVR_EXCL_LINE
        sese::net::Socket::close(event->fd);
        this->freeEvent(event);
    }
}

bool service::HttpService::onHandle(ByteBuilder *builder) noexcept {
    bool rt;
    net::http::RequestHeader req;
    net::http::ResponseHeader resp;
    resp.set("Server", config.servName);

    rt = net::http::HttpUtil::recvRequest(builder, &req);
    if (!rt) {
        return false;
    }

    auto url = net::http::Url(req.getUrl());

    auto iterator1 = config.controller1Map.find(url.getUrl());
    if (iterator1 != config.controller1Map.end()) {
        rt = iterator1->second(req, resp);
        if (!rt) {
            return false;
        }

        // 此处是向 buffer 中直接写入，故不需要判断是否成功
        net::http::HttpUtil::sendResponse(builder, &resp);
        return true;
    }

    auto iterator2 = config.controller2Map.find(url.getUrl());
    if (iterator2 != config.controller2Map.end()) {
        return iterator2->second(req, resp, builder);
    }

    // 忽略传输的内容
    char *end;
    auto len = std::strtol(req.get("Content-Length", "0").data(), &end, 10);
    builder->trunc(len);

    // 404
    resp.setCode(404);
    net::http::HttpUtil::sendResponse(builder, &resp);
    return true;
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