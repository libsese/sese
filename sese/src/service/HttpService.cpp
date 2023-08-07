#include "sese/service/HttpService.h"
#include "sese/net/http/HttpUtil.h"
#include "sese/net/http/UrlHelper.h"
#include "sese/text/DateTimeFormatter.h"
#include "sese/util/OutputUtil.h"
#include "sese/util/OutputBufferWrapper.h"
#include "sese/util/Util.h"

#include "openssl/ssl.h"

#include <filesystem>

using namespace sese;

using sese::net::http::Controller;
using sese::net::http::HttpConnection;
using sese::net::http::HttpHandleStatus;
using sese::net::http::Request;
using sese::net::http::Response;

/// https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

service::HttpConfig::HttpConfig() noexcept {
    this->otherController = {
            [](Request &req, Response &resp) {
                auto url = net::http::Url(req.getUrl());

                // 404
                resp.setCode(404);
                std::string content = "The controller or file named \"" + url.getUrl() + "\" could not be found.";
                req.getBody().write(content.c_str(), content.length());
                return true;
            }};
}

void service::HttpConfig::setController(const std::string &path, const Controller &controller) noexcept {
    this->controllerMap[path] = controller;
}

void service::HttpConfig::setController(const net::http::ControllerGroup &group) noexcept {
    auto &name = group.getName();
    auto &map = group.getControllerMap();
    for (auto &item: map) {
        this->controllerMap[name + item.first] = item.second;
    }
}

service::HttpService::HttpService(const HttpConfig &config) noexcept {
    this->config = config;
}

service::HttpService::~HttpService() noexcept {
    for (auto &item: eventMap) {
        auto event = item.second;
        auto conn = (HttpConnection *) event->data;
        if (conn->timeoutEvent) {
            this->freeTimeoutEvent(conn->timeoutEvent);
        }
        if (config.servCtx) {
            SSL_free((SSL *) conn->ssl);
        }
        sese::net::Socket::close(event->fd);
        delete conn;
        this->freeEvent(event);
    }
    eventMap.clear();
}

void service::HttpService::onAccept(int fd) {
    SSL *clientSSL = nullptr;

    // 一般不会失败
    // GCOVR_EXCL_START
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
        return;
    }
    // GCOVR_EXCL_STOP

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

    conn->event = this->createEventEx(fd, EVENT_READ, conn);
}

void service::HttpService::onRead(event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;

    // 如果服务器启用长连接，对端启用长连接，且是首次连接，则定时器存在
    // config.keepalive == true
    // conn->keepalive == false // 因为首次连接还没有经过 onHandle 处理
    // conn->timeoutEvent != nullptr

    // 如果服务器启用长连接，对端启用长连接，且不是首次连接，则定时器存在
    // config.keepalive == true
    // conn->keepalive == true // 已经经过 onHandle 处理
    // conn->timeoutEvent != nullptr

    // 如果服务器启用长连接，对端不启用长连接，且是首次连接，则定时存在
    // config.keepalive == true
    // conn->keepalive == false
    // conn->timeoutEvent != nullptr

    if (config.keepalive) {
        TimerableService::cancelTimeoutEvent(conn->timeoutEvent);
    }

    char buf[MTU_VALUE];
    while (true) {
        auto l = read(event->fd, buf, MTU_VALUE, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == ENOTCONN) {
                // 断开连接
                goto free;
            } else {
                // 无数据可读，触发子函数处理
                onHandle(conn);
                if (conn->status == HttpHandleStatus::OK ||
                    conn->status == HttpHandleStatus::FILE) {
                    // 准备进入写模式，返回响应
                    // event->events &= ~EVENT_READ;// 删除读事件
                    // event->events |= EVENT_WRITE;// 添加写事件
                    // this->setEvent(event);
                    // 此处应该手动触发而不是等待事件通知
                    onWrite(event);
                    break;
                } else {
                    // 处理错误，断开连接
                    goto free;
                }
            }
        } else {
            conn->req.getBody().write(buf, l);
            conn->requestSize += l;
        }
    }

    return;
free:
    if (conn->timeoutEvent) {
        TimerableService::freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEventEx(event);
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

void service::HttpService::onClose(sese::event::BaseEvent *event) {
    auto conn = (HttpConnection *) event->data;
    if (conn->timeoutEvent) {
        this->freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEventEx(event);
}

sese::event::BaseEvent *sese::service::HttpService::createEventEx(int fd, unsigned int events, void *data) noexcept {
    auto rt = this->createEvent(fd, events, data);
    if (rt) {
        this->eventMap[fd] = rt;
    }
    return rt;
}

void sese::service::HttpService::freeEventEx(sese::event::BaseEvent *event) noexcept {
    this->eventMap.erase(event->fd);
    this->freeEvent(event);
}

void service::HttpService::onHandle(HttpConnection *conn) noexcept {
    bool rt;
    conn->resp.set("Server", config.servName);

    rt = net::http::HttpUtil::recvRequest(&conn->req.getBody(), &conn->req);
    if (!rt) {
        conn->status = HttpHandleStatus::FAIL;
        return;
    }

    if (config.keepalive) {
        auto keepalive = sese::StrCmpI()(conn->req.get("Connection", "close").c_str(), "keep-alive") == 0;
        if (keepalive) {
            conn->resp.set("connection", "keep-alive");
            conn->resp.set("keep-alive", "timeout=" + std::to_string(config.keepalive) + " ,max=0");
        } else {
            this->freeTimeoutEvent(conn->timeoutEvent);
            conn->timeoutEvent = nullptr;
        }
    }

    conn->resp.set("accept-ranges", "bytes");
    conn->resp.set("date", sese::text::DateTimeFormatter::format(sese::DateTime::now(0), TIME_GREENWICH_MEAN_PATTERN));

    auto url = net::http::Url(conn->req.getUrl());

    auto iterator = config.controllerMap.find(url.getUrl());
    if (iterator != config.controllerMap.end()) {
        iterator->second(conn->req, conn->resp);
        conn->status = net::http::HttpHandleStatus::OK;
        conn->resp.set("content-length", std::to_string(conn->resp.getBody().getLength()));
        net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
        return;
    }

    if (!config.workDir.empty()) {
        auto file = config.workDir + url.getUrl();
        onHandleFile(conn, file);
    }

    if (conn->status == HttpHandleStatus::HANDING) {
        config.otherController(conn->req, conn->resp);
        conn->status = net::http::HttpHandleStatus::OK;
        conn->req.set("content-length", std::to_string(conn->resp.getBody().getLength()));
        net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
    }
}

void service::HttpService::onHandleFile(HttpConnection *conn, const std::string &path) noexcept {// NOLINT
    if (std::filesystem::is_regular_file(path)) {
        /// content-type 确定
        auto pos = path.find_last_of('.');
        if (pos != std::string::npos) {
            auto rawType = path.substr(pos + 1);
            auto iterator = contentTypeMap.find(rawType);
            if (iterator != contentTypeMap.end()) {
                conn->contentType = iterator->second;
            }
        }
        conn->resp.set("content-type", conn->contentType);

        /// last-modified
        auto lastModified = std::filesystem::last_write_time(path);
        uint64_t time = to_time_t(lastModified) * 1000 * 1000;
        conn->resp.set("last-modified", sese::text::DateTimeFormatter::format(sese::DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));

        conn->fileSize = std::filesystem::file_size(path);
        conn->ranges = sese::net::http::Range::parse(conn->req.get("Range", ""), conn->fileSize);
        if (conn->req.getType() == net::http::RequestType::Get) {
            conn->file = FileStream::create(path, "rb");
            if (conn->file == nullptr) {
                conn->status = net::http::HttpHandleStatus::OK;
                conn->resp.setCode(500);
                net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
                return;
            }
            // 完整文件
            if (conn->ranges.empty()) {
                conn->ranges.emplace_back(0, conn->fileSize);
                conn->rangeIterator = conn->ranges.begin();
                conn->resp.set("content-length", std::to_string(conn->fileSize));
                net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
                conn->status = HttpHandleStatus::FILE;
                return;
            }
            // 分块请求
            conn->resp.setCode(206);
            conn->rangeIterator = conn->ranges.begin();
            if (conn->ranges.size() > 1) {
                size_t len = 0;
                for (auto item: conn->ranges) {
                    len += 4;// \r\n--
                    len += strlen(HTTPD_BOUNDARY);
                    len += 2;// \r\n
                    len += strlen("Content-Type: ");
                    len += conn->contentType.length();
                    len += 2;// \r\n
                    len += strlen("Content-Range: ");
                    len += item.toString(conn->fileSize).length();
                    len += 4;// \r\n\r\n
                    len += item.len;
                }
                len += 4;// \r\n--
                len += strlen(HTTPD_BOUNDARY);
                len += 4;// --\r\n
                len -= 2;// 抵消首次区块的 \r\n

                conn->resp.set("content-length", std::to_string(len));
                conn->resp.set("content-type", std::string("multipart/byteranges; boundary=") + HTTPD_BOUNDARY);
                net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);

                conn->buffer << "--";
                conn->buffer << HTTPD_BOUNDARY;
                conn->buffer << "\r\ncontent-type: ";
                conn->buffer << conn->contentType;
                conn->buffer << "\r\ncontent-range: ";
                conn->buffer << conn->rangeIterator->toString(conn->fileSize);
                conn->buffer << "\r\n\r\n";
                conn->file->setSeek((int64_t) conn->rangeIterator->begin, SEEK_SET);
            } else {
                conn->file->setSeek((int64_t) conn->rangeIterator->begin, SEEK_SET);
                conn->resp.set("content-length", std::to_string(conn->rangeIterator->len));
                conn->resp.set("content-range", conn->rangeIterator->toString(conn->fileSize));
                net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
            }

            conn->status = HttpHandleStatus::FILE;
            return;
        } else if (conn->req.getType() == net::http::RequestType::Head) {
            conn->resp.setCode(200);
            conn->resp.set("content-length", std::to_string(conn->fileSize));
            net::http::HttpUtil::sendResponse(&conn->buffer, &conn->resp);
            conn->status = HttpHandleStatus::OK;
            return;
        }
    }
    // 文件若是非常规文件或是无法打开文件
    // 此处不会改变状态，留给 otherController 处理
    conn->status = HttpHandleStatus::HANDING;
}

void service::HttpService::onControllerWrite(event::BaseEvent *event) noexcept {
    auto conn = (HttpConnection *) event->data;
    char buf[MTU_VALUE]{};
    // 先发送头部
    while (true) {
        auto len = conn->buffer.peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED) {
                // 不能继续写入，等待下一次触发可写事件
                conn->event->events &= ~EVENT_READ;
                conn->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->buffer.trunc(l);
        }
    }
    // 发送内容
    while (true) {
        decltype(auto) body = conn->resp.getBody();
        auto len = body.peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED) {
                // 不能继续写入，等待下一次触发可写事件
                conn->event->events &= ~EVENT_READ;
                conn->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            body.trunc(l);
        }
    }
    if (conn->timeoutEvent) {
        conn->buffer.freeCapacity();
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
    if (conn->timeoutEvent) {
        this->freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEventEx(event);
}

void service::HttpService::onFileWrite(event::BaseEvent *event) noexcept {
    auto conn = (HttpConnection *) event->data;
    char buf[MTU_VALUE]{};
    // 先发送头部
    while (true) {
        auto len = conn->buffer.peek(buf, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED) {
                // 不能继续写入，等待下一次触发可写事件
                conn->event->events &= ~EVENT_READ;
                conn->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->buffer.trunc(l);
        }
    }
    // 发送内容
    while (true) {
        // auto need = conn->responseBodySize - conn->responseBodyWroteSize;
        auto need = conn->rangeIterator->len - conn->rangeIterator->cur;
        need = need >= MTU_VALUE ? MTU_VALUE : need % MTU_VALUE;
        if (need == 0) {
            break;
        }
        auto len = conn->file->peek(buf, need);
        auto l = write(event->fd, buf, len, conn->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR || err == ECONNABORTED) {
                // 不能继续写入，等待下一次触发可写事件
                conn->event->events &= ~EVENT_READ;
                conn->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                // 断开连接...等
                goto free;
            }
        } else {
            conn->file->trunc(l);
            // conn->responseBodyWroteSize += l;
            conn->rangeIterator->cur += l;
        }
    }
    // if (conn->responseBodySize == conn->responseBodyWroteSize) {
    if (conn->rangeIterator->len == conn->rangeIterator->cur) {
        // 处理下一个区间
        if (conn->rangeIterator + 1 != conn->ranges.end()) {
            conn->rangeIterator++;

            conn->file->setSeek((int64_t) conn->rangeIterator->begin, SEEK_SET);

            conn->buffer << "\r\n--";
            conn->buffer << HTTPD_BOUNDARY;
            conn->buffer << "\r\nContent-Type: ";
            conn->buffer << conn->contentType;
            conn->buffer << "\r\nContent-Range: ";
            conn->buffer << conn->rangeIterator->toString(conn->fileSize);
            conn->buffer << "\r\n\r\n";

            this->setEvent(conn->event);
            return;
        }
        // 已是最后一个区间
        // else {
        else {
            if (conn->ranges.size() > 1) {
                char buffer[MTU_VALUE]{};
                auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));

                output << "\r\n--";
                output << HTTPD_BOUNDARY;
                output << "--\r\n";

                write(event->fd, buffer, output.getLen(), conn->ssl);
            } else {
            }
        }
        // 进入下一次请求
        if (conn->timeoutEvent) {
            conn->buffer.freeCapacity();
            conn->file->close();
            conn->file = nullptr;
            this->setTimeoutEvent(conn->timeoutEvent, config.keepalive);
            conn->event->events &= ~EVENT_WRITE;
            conn->event->events |= EVENT_READ;
            this->setEvent(conn->event);
        }
        // 断开连接
        else {
            goto free;
        }
    }

    return;
free:
    if (conn->timeoutEvent) {
        this->freeTimeoutEvent(conn->timeoutEvent);
    }
    if (config.servCtx) {
        SSL_free((SSL *) conn->ssl);
    }
    sese::net::Socket::close(event->fd);
    delete conn;
    this->freeEventEx(event);
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
    this->freeEventEx(conn->event);
    delete conn;
}
