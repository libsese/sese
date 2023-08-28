#include <sese/service/HttpService_V2.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/net/http/UrlHelper.h>
#include <sese/net/http/HPackUtil.h>
#include <sese/text/DateTimeFormatter.h>
#include <sese/util/OutputBufferWrapper.h>
#include <sese/util/Util.h>

#include <sese/record/Marco.h>
#define printf SESE_INFO

#include <openssl/ssl.h>

#include <algorithm>
#include <filesystem>

#define CONFIG ((sese::service::v2::HttpConfig *) this->config)

void sese::service::v2::HttpConfig::otherController(net::http::Request &req, net::http::Response &resp) {
    auto url = net::http::Url(req.getUrl());
    // 404
    resp.setCode(404);
    std::string content = "The controller or file named \"" + url.getUrl() + "\" could not be found.";
    resp.getBody().write(content.c_str(), content.length());
}

void sese::service::v2::HttpConfig::setController(const std::string &path, const net::http::Controller &controller) {
    controllerMap[path] = controller;
}

void sese::service::v2::HttpConfig::setController(const net::http::ControllerGroup &group) {
    auto &name = group.getName();
    auto &map = group.getControllerMap();
    for (auto &item: map) {
        this->controllerMap[name + item.first] = item.second;
    }
}

sese::service::TcpConnection *sese::service::v2::HttpConfig::createConnection() {
    return new HttpConnectionWrapper;
}

sese::service::v2::HttpService::HttpService(sese::service::TcpTransporterConfig *transporterConfig) : TcpTransporter(transporterConfig) {}

int sese::service::v2::HttpService::onProcAlpnSelect(const uint8_t **out, uint8_t *outLength, const uint8_t *in, uint32_t inLength) {
    if (SSL_select_next_proto((unsigned char **) out, outLength, (const uint8_t *) CONFIG->servProtos.c_str(), (uint32_t) CONFIG->servProtos.length(), in, inLength) != OPENSSL_NPN_NEGOTIATED) {
        return SSL_TLSEXT_ERR_NOACK;
    }
    return SSL_TLSEXT_ERR_OK;
}

void sese::service::v2::HttpService::onProcAlpnGet(sese::service::TcpConnection *conn, const uint8_t *in, uint32_t inLength) {
    auto wrapper = (HttpConnectionWrapper *) conn;
    std::string proto{(const char *) in, inLength};
    if (proto == "h2") {
        // http 2
        wrapper->conn = std::make_shared<Http2Connection>();
    } else {
        // http 1.1
        wrapper->conn = std::make_shared<Http1_1Connection>();
    }
}

void sese::service::v2::HttpService::onProcHandle(sese::service::TcpConnection *conn) {
    auto wrapper = (HttpConnectionWrapper *) conn;
    if (!wrapper->conn) {
        // 说明没有经过 ALPN 协商
        wrapper->conn = std::make_shared<Http1_1Connection>();
        onProcHandle1_1(conn);
    } else if (wrapper->conn->version == net::http::HttpVersion::VERSION_1_1) {
        // http/1.1
        onProcHandle1_1(conn);
    } else {
        // http 2
        onProcHandle2(conn);
    }
}

void sese::service::v2::HttpService::onProcClose(sese::service::TcpConnection *conn) {
    // printf("on close %d", sese::net::getNetworkError());
}

void sese::service::v2::HttpService::onWrite(sese::event::BaseEvent *event) {
    auto wrapper = (HttpConnectionWrapper *) event->data;
    if (wrapper->conn->version == net::http::HttpVersion::VERSION_1_1) {
        auto httpConn = std::dynamic_pointer_cast<Http1_1Connection>(wrapper->conn);
        if (httpConn->status == net::http::HttpHandleStatus::OK) {
            // controller
            TcpTransporter::onWrite(event);
        } else if (httpConn->status == net::http::HttpHandleStatus::FILE) {
            // file
            onWriteFile1_1(event);
        } else if (httpConn->status == net::http::HttpHandleStatus::FAIL) {
            // close
            if (wrapper->timeoutEvent) {
                TimerableService::freeTimeoutEvent(wrapper->timeoutEvent);
            }
            onProcClose(wrapper);
            if (config->servCtx) {
                SSL_free((SSL *) wrapper->ssl);
            }
            sese::net::Socket::close(event->fd);
            config->freeConnection(wrapper);
            this->freeEventEx(event);
        } else {
            // 不可达
        }
    } else {
        if (wrapper->conn->upgrade) {
            // 此连接正在升级
            TcpTransporter::onWrite(event);
        } else {
            // 安装 http 2 处理
            onWrite2(event);
        }
    }
}

void sese::service::v2::HttpService::onWriteFile1_1(event::BaseEvent *event) noexcept {
    // printf("write");
    auto wrapper = (HttpConnectionWrapper *) event->data;
    auto httpConn = std::dynamic_pointer_cast<Http1_1Connection>(wrapper->conn);
    char buffer[MTU_VALUE]{};
    while (true) {
        auto len = wrapper->buffer2write.peek(buffer, MTU_VALUE);
        if (len == 0) {
            break;
        }
        auto l = write(event->fd, buffer, len, wrapper->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR) {
                // 不能继续写入，等待下一次触发可写事件
                wrapper->event->events &= ~EVENT_READ;
                wrapper->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                goto free;
            }
        } else {
            wrapper->buffer2write.trunc(l);
        }
    }

    while (true) {
        auto need = httpConn->rangeIterator->len - httpConn->rangeIterator->cur;
        need = need >= MTU_VALUE ? MTU_VALUE : need;
        if (need == 0) {
            break;
        }
        auto len = httpConn->file->read(buffer, need);
        auto l = write(event->fd, buffer, len, wrapper->ssl);
        if (l <= 0) {
            auto err = sese::net::getNetworkError();
            if (err == EWOULDBLOCK || err == EINTR) {
                // 不能继续写入，等待下一次触发可写事件
                // printf("blocking %d", err);
                // 本次未能发送，回滚 need 字节
                // printf("rollback %d bytes", (int) (need));
                httpConn->file->setSeek((int64_t) (0 - need), SEEK_CUR);
                wrapper->event->events &= ~EVENT_READ;
                wrapper->event->events |= EVENT_WRITE;
                this->setEvent(event);
                return;
            } else {
                // printf("errno %d", err);
                goto free;
            }
        } else {
            if (l < len) {
                // printf("rollback %d bytes", (int) (len - l));
                // 本次未能完全发送完，需要回滚 len - l 字节
                httpConn->file->setSeek(l - len, SEEK_CUR);
            }
            httpConn->rangeIterator->cur += l;
        }
    }

    if (httpConn->rangeIterator + 1 != httpConn->ranges.end()) {
        // 处理下一个区间
        httpConn->rangeIterator += 1;
        httpConn->file->setSeek((int64_t) httpConn->rangeIterator->begin, SEEK_SET);
        wrapper->buffer2write.write("\r\n--", 4);
        wrapper->buffer2write.write(HTTPD_BOUNDARY, strlen(HTTPD_BOUNDARY));
        wrapper->buffer2write.write("\r\nContent-Type: ", 16);
        wrapper->buffer2write.write(httpConn->contentType.c_str(), httpConn->contentType.length());
        wrapper->buffer2write.write("\r\nContent-Range: ", 17);
        auto contentRange = httpConn->rangeIterator->toString(httpConn->fileSize);
        wrapper->buffer2write.write(contentRange.c_str(), contentRange.length());
        wrapper->buffer2write.write("\r\n\r\n", 4);
        wrapper->event->events &= ~EVENT_READ;
        wrapper->event->events |= EVENT_WRITE;
        this->setEvent(wrapper->event);
        // postWrite(wrapper);
        return;
    } else {
        // 已是最后一个区间且存在多个区间
        if (httpConn->ranges.size() > 1) {
            auto output = sese::OutputBufferWrapper(buffer, sizeof(buffer));
            output.write("\r\n--", 4);
            output.write(HTTPD_BOUNDARY, strlen(HTTPD_BOUNDARY));
            output.write("--\r\n", 4);
            // 数据很少，不考虑发送失败的情况
            write(event->fd, buffer, output.getLength(), wrapper->ssl);
        }
    }

    // printf("finsh %s", httpConn->contentType.c_str());
    if (wrapper->timeoutEvent) {
        wrapper->buffer2write.freeCapacity();
        this->setTimeoutEvent(wrapper->timeoutEvent, config->keepalive);
        wrapper->event->events &= ~EVENT_WRITE;
        wrapper->event->events |= EVENT_READ;
        this->setEvent(wrapper->event);
        return;
    } else {
        goto free;
    }

free:
    if (wrapper->timeoutEvent) {
        this->freeTimeoutEvent(wrapper->timeoutEvent);
    }
    onProcClose(wrapper);
    if (CONFIG->servCtx) {
        SSL_free((SSL *) wrapper->ssl);
    }
    sese::net::Socket::close(wrapper->event->fd);
    CONFIG->freeConnection(wrapper);
    freeEventEx(event);
}

void sese::service::v2::HttpService::onProcHandle1_1(sese::service::TcpConnection *conn) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http1_1Connection>(wrapper->conn);

    // 重置状态
    httpConn->status = net::http::HttpHandleStatus::HANDING;

    // 预期值为 0 代表是请求头
    if (httpConn->expect == 0) {
        auto rt = net::http::HttpUtil::recvRequest(&conn->buffer2read, &httpConn->req);
        if (!rt) {
            httpConn->status = net::http::HttpHandleStatus::FAIL;
            postWrite(conn);
            return;
        }
        char *end;
        httpConn->expect = std::strtoul(httpConn->req.get("content-length", "0").c_str(), &end, 10);

        auto connectString = httpConn->req.get("Connection", "close");
        bool found = false;
        auto connOptions = text::StringBuilder::split(connectString, ", ");
        for (auto &str: connOptions) {
            if (strcmpDoNotCase(str.c_str(), "upgrade")) {
                found = true;
                break;
            }
        }
        if (found) {
            httpConn->upgrade = true;
        } else {
            if (config->keepalive) {
                if (sese::strcmpDoNotCase(connectString.c_str(), "keep-alive")) {
                    httpConn->resp.set("connection", "keep-alive");
                    httpConn->resp.set("keep-alive", "timeout=" + std::to_string(config->keepalive) + " ,max=0");
                } else {
                    httpConn->resp.set("connection", "close");
                    this->freeTimeoutEvent(conn->timeoutEvent);
                    conn->timeoutEvent = nullptr;
                }
            }
        }
    }
    // 预期值与实际值不相等，说明 body 长度不够
    if (httpConn->expect != httpConn->real) {
        httpConn->real += (uint32_t) streamMove(&httpConn->req.getBody(), &conn->buffer2read, httpConn->expect - httpConn->real);
        if (httpConn->expect != httpConn->real) {
            conn->buffer2read.freeCapacity();
            // printf("postRead");
            postRead(conn);
            return;
        } else {
            httpConn->expect = 0;
            httpConn->real = 0;
        }
    }

    httpConn->resp.set("server", CONFIG->servName);
    httpConn->resp.set("accept-ranges", "bytes");
    httpConn->resp.set("date", text::DateTimeFormatter::format(DateTime::now(0), TIME_GREENWICH_MEAN_PATTERN));

    // 判断是否需要升级连接
    if (httpConn->upgrade) {
        if (onProcUpgrade2(conn)) {
            // 升级成功则不需要后续处理，
            // 仅进行最后一次 http/1.1 响应
            postWrite(conn);
            return;
        }
        // 未升级成功则依照 http/1.1 继续处理
    }

    /// http 1.1 控制器
    auto url = net::http::Url(httpConn->req.getUrl());
    // printf("%s", url.getUrl().c_str());
    auto iterator = CONFIG->controllerMap.find(url.getUrl());
    if (iterator != CONFIG->controllerMap.end()) {
        iterator->second(httpConn->req, httpConn->resp);
        auto contentLength = httpConn->resp.getBody().getReadableSize();
        httpConn->resp.set("content-length", std::to_string(contentLength));
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);
        if (contentLength != 0) {
            streamMove(&conn->buffer2write, &httpConn->resp.getBody(), contentLength);
        }
        httpConn->status = net::http::HttpHandleStatus::OK;

        httpConn->req.clear();
        httpConn->resp.getBody().freeCapacity();
        httpConn->resp.clear();
        httpConn->resp.getBody().freeCapacity();
    }

    /// http 1.1 文件传输
    if (httpConn->status == net::http::HttpHandleStatus::HANDING && !CONFIG->workDir.empty()) {
        auto file = CONFIG->workDir + url.getUrl();
        if (std::filesystem::is_regular_file(file)) {
            // printf("file %s", file.c_str());
            onProcHandleFile1_1(file, conn);
        }
    }

    /// http 1.1 默认控制器
    if (httpConn->status == net::http::HttpHandleStatus::HANDING) {
        CONFIG->otherController(httpConn->req, httpConn->resp);
        auto contentLength = httpConn->resp.getBody().getReadableSize();
        httpConn->resp.set("content-length", std::to_string(contentLength));
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);
        if (contentLength != 0) {
            streamMove(&conn->buffer2write, &httpConn->resp.getBody(), contentLength);
        }
        httpConn->status = net::http::HttpHandleStatus::OK;

        httpConn->req.clear();
        httpConn->resp.getBody().freeCapacity();
        httpConn->resp.clear();
        httpConn->resp.getBody().freeCapacity();
    }

    postWrite(conn);
}

void sese::service::v2::HttpService::onProcHandleFile1_1(const std::string &path, sese::service::TcpConnection *conn) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http1_1Connection>(wrapper->conn);

    if (httpConn->req.getType() != net::http::RequestType::Get) {
        httpConn->status = net::http::HttpHandleStatus::HANDING;
        return;
    }

    auto pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        auto rawType = path.substr(pos + 1);
        auto iterator = net::http::HttpUtil::contentTypeMap.find(rawType);
        if (iterator != net::http::HttpUtil::contentTypeMap.end()) {
            httpConn->contentType = iterator->second;
        }
    }
    httpConn->resp.set("content-type", httpConn->contentType);

    auto lastModified = std::filesystem::last_write_time(path);
    uint64_t time = to_time_t(lastModified) * 1000 * 1000;
    httpConn->resp.set("last-modified", sese::text::DateTimeFormatter::format(sese::DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));
    httpConn->fileSize = std::filesystem::file_size(path);
    httpConn->ranges = sese::net::http::Range::parse(httpConn->req.get("Range", ""), httpConn->fileSize);

    httpConn->file = FileStream::create(path, "rb");
    if (httpConn->file == nullptr) {
        httpConn->status = net::http::HttpHandleStatus::OK;
        httpConn->resp.setCode(500);
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);
        return;
    }
    if (httpConn->ranges.empty()) {
        httpConn->status = net::http::HttpHandleStatus::FILE;
        httpConn->ranges.emplace_back(0, httpConn->fileSize);
        httpConn->rangeIterator = httpConn->ranges.begin();
        httpConn->resp.setCode(200);
        httpConn->resp.set("content-length", std::to_string(httpConn->fileSize));
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);
        return;
    }

    httpConn->resp.setCode(206);
    httpConn->rangeIterator = httpConn->ranges.begin();
    if (httpConn->ranges.size() > 1) {
        size_t len = 0;
        for (auto item: httpConn->ranges) {
            len += 12 +
                   strlen(HTTPD_BOUNDARY) +
                   strlen("Content-Type: ") +
                   httpConn->contentType.length() +
                   strlen("Content-Range: ") +
                   item.toString(httpConn->fileSize).length() +
                   item.len;
        }
        len += 6 + strlen(HTTPD_BOUNDARY);

        httpConn->resp.set("content-length", std::to_string(len));
        httpConn->resp.set("content-type", std::string("multipart/byteranges; boundary=") + HTTPD_BOUNDARY);
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);

        conn->buffer2write.write("--", 2);
        conn->buffer2write.write(HTTPD_BOUNDARY, strlen(HTTPD_BOUNDARY));
        conn->buffer2write.write("\r\ncontent-type: ", 16);
        conn->buffer2write.write(httpConn->contentType.c_str(), httpConn->contentType.length());
        conn->buffer2write.write("\r\ncontent-range: ", 17);
        auto contentRange = httpConn->rangeIterator->toString(httpConn->fileSize);
        conn->buffer2write.write(contentRange.c_str(), contentRange.length());
        conn->buffer2write.write("\r\n\r\n", 4);
        httpConn->file->setSeek((int64_t) httpConn->rangeIterator->begin, SEEK_SET);
        httpConn->status = net::http::HttpHandleStatus::FILE;
        return;
    } else {
        httpConn->file->setSeek((int64_t) httpConn->rangeIterator->begin, SEEK_SET);
        httpConn->resp.set("content-length", std::to_string(httpConn->rangeIterator->len));
        httpConn->resp.set("content-range", httpConn->rangeIterator->toString(httpConn->fileSize));
        net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);
        httpConn->status = net::http::HttpHandleStatus::FILE;
        return;
    }
}

void sese::service::v2::HttpService::onWrite2(event::BaseEvent *event) noexcept {
    auto wrapper = (HttpConnectionWrapper *) event->data;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    // TcpTransporter::onWrite(event);
    {
        char buf[MTU_VALUE];
        while (true) {
            auto len = wrapper->buffer2write.peek(buf, MTU_VALUE);
            if (len == 0) {
                wrapper->buffer2write.freeCapacity();
                break;
            }
            auto l = write(event->fd, buf, len, wrapper->ssl);
            if (l <= 0) {
                auto err = sese::net::getNetworkError();
                if (err == EWOULDBLOCK || err == EINTR) {
                    wrapper->event->events &= ~EVENT_READ;
                    wrapper->event->events |= EVENT_WRITE;
                    this->setEvent(event);
                    break;
                } else {
                    goto free;
                }
            } else {
                wrapper->buffer2write.trunc(l);
            }
        }
    }

    for (auto iterator = httpConn->streamMap.begin(); iterator != httpConn->streamMap.end();) {
        auto &stream = iterator->second;
        // 该流被重置，直接移除
        if (stream->reset) {
            httpConn->streamMap.erase(iterator++);
            continue;
        }
        // 该流是文件流，需要特殊处理
        // 在此实现中，为了方便处理，发送的文件流中区间和帧的关系是 1 : N
        // 也就是说一个区间至少占用一个帧
        // 单区间文件
        if (stream->status == net::http::HttpHandleStatus::FILE && stream->ranges.size() == 1) {
            size_t offset = 0;
            char buffer[MTU_VALUE];
            // 说明是一个帧的帧头
            if (stream->currentFramePos == 0) {
                stream->currentFrameSize = std::min<size_t>(stream->contentLength, httpConn->maxFrameSize);
                offset = 9;
                net::http::Http2FrameInfo frame{};
                frame.ident = stream->id;
                frame.type = net::http::FRAME_TYPE_DATA;
                frame.flags = stream->currentFrameSize == stream->contentLength ? net::http::FRAME_FLAG_END_STREAM : 0;
                frame.length = (uint32_t) stream->currentFrameSize;
                writeFrame((uint8_t *) buffer, frame);

                if (stream->rangeIterator->cur == 0) {
                    stream->file->setSeek((int64_t) stream->rangeIterator->begin, SEEK_SET);
                }
            }
            while (true) {
                auto need = std::min<size_t>({stream->contentLength, MTU_VALUE - offset, stream->currentFrameSize - stream->currentFramePos});
                auto len = stream->file->read(buffer + offset, need);
                auto l = write(wrapper->event->fd, buffer, len + offset, wrapper->ssl);
                if (l < 0) {
                    auto err = sese::net::getNetworkError();
                    if (err == EWOULDBLOCK || err == EINTR) {
                        stream->file->setSeek((int64_t) (0 - need), SEEK_CUR);
                        event->events |= EVENT_WRITE;
                        setEvent(event);
                        return;
                    } else {
                        goto free;
                    }
                } else {
                    // warning: 此处不考虑帧头发送一半的可能性
                    if (l < (int) offset) {
                        // 几乎不可能发生
                        goto free;
                    } else {
                        // 内容不完全
                        if (l < (int) (len + offset)) {
                            auto rollback = (l - offset) - len;
                            stream->file->setSeek((int64_t) rollback, SEEK_CUR);
                        }
                        stream->currentFramePos += (uint32_t) (l - offset);
                        stream->rangeIterator->cur += l - offset;
                        stream->contentLength -= l - offset;
                        offset = 0;
                    }
                }
                if (stream->contentLength == 0) {
                    // 内容发送完成
                    // 已无可发送区间
                    // 移除当前流
                    httpConn->streamMap.erase(iterator++);
                    break;
                } else if (stream->currentFramePos == stream->currentFrameSize) {
                    // 重置状态，组织新帧
                    stream->currentFramePos = 0;
                    stream->currentFrameSize = 0;
                    event->events |= EVENT_WRITE;
                    setEvent(event);
                    break;
                }
            }
        }
        // 多区间文件
        else if (stream->status == net::http::HttpHandleStatus::FILE && stream->ranges.size() > 1) {
            // 此偏移不计入 content 总长度
            size_t offset0 = 0;
            // 此偏移计入 content 总长度
            size_t offset1 = 0;
            char buffer[MTU_VALUE];
            // 说明是一个帧的帧头
            if (stream->currentFramePos == 0) {
                // 说明本帧是分段的起始帧，需要添加额外信息
                if (stream->rangeIterator->cur == 0) {
                    auto output = sese::OutputBufferWrapper(buffer + 9, MTU_VALUE - 9);
                    output.write("\r\n--", 4);
                    output.write(HTTPD_BOUNDARY, strlen(HTTPD_BOUNDARY));
                    output.write("\r\ncontent-type: ", 16);
                    output.write(stream->contentType.c_str(), stream->contentType.length());
                    output.write("\r\ncontent-range: ", 17);
                    auto contentRange = stream->rangeIterator->toString(stream->fileSize);
                    output.write(contentRange.c_str(), contentRange.length());
                    output.write("\r\n\r\n", 4);
                    stream->file->setSeek((int64_t) stream->rangeIterator->begin, SEEK_SET);
                    offset1 = output.getLength();
                }

                stream->currentFrameSize = std::min<size_t>({stream->contentLength, httpConn->maxFrameSize, stream->rangeIterator->len - stream->rangeIterator->cur + offset1});
                offset0 = 9;
                net::http::Http2FrameInfo frame{};
                frame.ident = stream->id;
                frame.type = net::http::FRAME_TYPE_DATA;
                frame.length = (uint32_t) stream->currentFrameSize;
                writeFrame((uint8_t *) buffer, frame);
            }
            while (true) {
                auto need = std::min<size_t>({stream->contentLength, MTU_VALUE - offset0 - offset1, stream->currentFrameSize - stream->currentFramePos - offset1});
                // 此块内容是最后一帧的最后一部分内容，需要进行修正
                if (need + offset1 == stream->contentLength) {
                    need -= 8 + strlen(HTTPD_BOUNDARY);
                }
                auto len = stream->file->read(buffer + offset0 + offset1, need);
                auto l = write(wrapper->event->fd, buffer, len + offset0 + offset1, wrapper->ssl);
                if (l < 0) {
                    auto err = sese::net::getNetworkError();
                    if (err == EWOULDBLOCK || err == EINTR) {
                        stream->file->setSeek((int64_t) (0 - need), SEEK_CUR);
                        event->events |= EVENT_WRITE;
                        setEvent(event);
                        return;
                    } else {
                        goto free;
                    }
                } else {
                    if (l < (int) (offset0 + offset1)) {
                        // 几乎不可能发生
                        goto free;
                    } else {
                        // 内容不完全
                        if (l < (int) (len + offset0 + offset1)) {
                            auto rollback = (l - offset0 - offset1) - len;
                            stream->file->setSeek((int64_t) rollback, SEEK_CUR);
                        }
                        stream->currentFramePos += l - offset0;
                        stream->rangeIterator->cur += l - offset0 - offset1;
                        stream->contentLength -= l - offset0;
                        offset0 = 0;
                        offset1 = 0;
                    }
                }

                // 正常帧结束
                if (stream->rangeIterator->cur == stream->rangeIterator->len) {
                    break;
                }
            }

            // 此处判断是否为最后一帧预备结束
            if (stream->rangeIterator + 1 != stream->ranges.end()) {
                // 不是最后一帧预备结束
                // 重置状态，组织新帧
                stream->rangeIterator++;
                stream->currentFramePos = 0;
                stream->currentFrameSize = 0;
                event->events |= EVENT_WRITE;
                setEvent(event);
                break;
            } else {
                // 最后一帧，发送尾部 BOUNDARY 并移除当前流
                net::http::Http2FrameInfo frame{};
                frame.ident = stream->id;
                frame.length = (uint32_t) (8 + strlen(HTTPD_BOUNDARY));
                frame.type = net::http::FRAME_TYPE_DATA;
                frame.flags = net::http::FRAME_FLAG_END_STREAM;
                wrapper->writeFrame(frame);
                wrapper->buffer2write.write("\r\n--", 4);
                wrapper->buffer2write.write(HTTPD_BOUNDARY, strlen(HTTPD_BOUNDARY));
                wrapper->buffer2write.write("--\r\n", 4);
                httpConn->streamMap.erase(iterator++);
                TcpTransporter::onWrite(event);
                break;
            }
        }
        // 可能是其他未处理完成并未被重置的流
        else {
            iterator++;
        }
    }

    if (wrapper->timeoutEvent) {
        this->setTimeoutEvent(wrapper->timeoutEvent, config->keepalive);
        event->events &= ~EVENT_WRITE;
        event->events |= EVENT_READ;
        setEvent(event);
    }
    return;
free:
    if (wrapper->timeoutEvent) {
        this->freeTimeoutEvent(wrapper->timeoutEvent);
    }
    onProcClose(wrapper);
    if (CONFIG->servCtx) {
        SSL_free((SSL *) wrapper->ssl);
    }
    sese::net::Socket::close(wrapper->event->fd);
    CONFIG->freeConnection(wrapper);
    freeEventEx(event);
}

bool sese::service::v2::HttpService::onProcUpgrade2(sese::service::TcpConnection *conn) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http1_1Connection>(wrapper->conn);

    const char *upgradeStringSsl = "h2";
    const char *upgradeStringNoSsl = "h2c";
    const char *expectUpgradeString = upgradeStringSsl;
    if (!CONFIG->servCtx) {
        expectUpgradeString = upgradeStringNoSsl;
    }

    auto currentUpgradeString = httpConn->req.get("upgrade", "undef");
    if (!strcmpDoNotCase(expectUpgradeString, currentUpgradeString.c_str())) {
        httpConn->upgrade = false;
        return false;
    }

    bool upgradeStatus = false;
    bool settingsStatus = false;
    auto connString = httpConn->req.get("connection", "undef");
    if (connString != "undef") {
        auto connVector = text::StringBuilder::split(connString, ", ");
        for (auto &str: connVector) {
            if (strcmpDoNotCase(str.c_str(), "upgrade")) {
                upgradeStatus = true;
            } else if (strcmpDoNotCase(str.c_str(), "http2-settings")) {
                settingsStatus = true;
            }
        }
    }

    if (!upgradeStatus) {
        httpConn->upgrade = false;
        return false;
    }

    // 准备最后一次响应内容
    httpConn->resp.setCode(101);
    httpConn->resp.set("connection", "upgrade");
    httpConn->resp.set("upgrade", expectUpgradeString);
    net::http::HttpUtil::sendResponse(&conn->buffer2write, &httpConn->resp);

    auto http2Conn = std::make_shared<Http2Connection>();
    http2Conn->upgrade = true;
    if (settingsStatus) {
        http2Conn->decodeHttp2Settings(httpConn->req.get("http2-settings", ""));
    }

    // 当前请求移交给 stream id 为 1 的流
    auto stream = std::make_shared<Http2Stream>();
    stream->id = 1;
    stream->req.swap(httpConn->req);
    stream->req.setVersion(net::http::HttpVersion::VERSION_2);
    stream->resp.setVersion(net::http::HttpVersion::VERSION_2);
    http2Conn->streamMap[1] = stream;
    wrapper->conn = http2Conn;
    return true;
}

void sese::service::v2::HttpService::onProcHandle2(TcpConnection *conn) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    net::http::Http2FrameInfo frame{};
    if (httpConn->first) {
        // 此链接为首次连接，需要处理 MAGIC STRING
        httpConn->first = false;
        char buffer[25]{};
        conn->buffer2read.read(buffer, 24);
        frame.type = net::http::FRAME_TYPE_SETTINGS;
        wrapper->writeFrame(frame);
    }
    if (httpConn->upgrade) {
        // 此连接由 http/1.1 升级而来
        httpConn->upgrade = false;
        auto stream = httpConn->streamMap[1];
        onProcDispatch2(conn, stream);
    }

    while (wrapper->readFrame(frame)) {
        if (frame.type == net::http::FRAME_TYPE_SETTINGS) {
            onSettingsFrame(conn, frame);
            if (frame.flags != net::http::SETTINGS_FLAGS_ACK) {
                wrapper->writeAck();
            }
            continue;
        } else if (frame.type == net::http::FRAME_TYPE_WINDOW_UPDATE) {
            onWindowUpdate(conn, frame);
            continue;
        } else if (frame.type == net::http::FRAME_TYPE_PRIORITY) {
            conn->buffer2read.trunc(5);
            continue;
        } else if (frame.type == net::http::FRAME_TYPE_HEADERS || frame.type == net::http::FRAME_TYPE_CONTINUATION) {
            if (frame.flags & net::http::FRAME_FLAG_PRIORITY) {
                conn->buffer2read.trunc(5);
            }
            onHeaderFrame(conn, frame);
            continue;
        } else if (frame.type == net::http::FRAME_TYPE_DATA) {
            onDataFrame(conn, frame);
            continue;
        } else if (frame.type == net::http::FRAME_TYPE_RST_STREAM) {
            onResetFrame(conn, frame);
            continue;
        }
    }

    wrapper->event->events |= EVENT_READ;
    // wrapper->event->events |= EVENT_WRITE;
    setEvent(wrapper->event);
    onWrite2(wrapper->event);
}

void sese::service::v2::HttpService::onProcDispatch2(sese::service::TcpConnection *conn, const sese::service::v2::Http2Stream::Ptr &stream) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    stream->resp.set("accept-ranges", "bytes");
    stream->resp.set("date", sese::text::DateTimeFormatter::format(sese::DateTime::now(0), TIME_GREENWICH_MEAN_PATTERN));
    stream->resp.set("server", CONFIG->servName);

    // http2 控制器
    auto url = net::http::Url(stream->req.getUrl());
    auto iterator = CONFIG->controllerMap.find(url.getUrl());
    if (iterator != CONFIG->controllerMap.end()) {
        iterator->second(stream->req, stream->resp);
        auto len = stream->resp.getBody().getReadableSize();
        stream->resp.set("content-length", std::to_string(len));
        // response2Http2(stream->resp);
        writeHeader(conn, stream);
        if (len) {
            writeData(conn, stream);
        }
        stream->status = net::http::HttpHandleStatus::OK;
        // 流生存周期结束
        httpConn->streamMap.erase(stream->id);
    }

    // 文件处理
    if (stream->status == net::http::HttpHandleStatus::HANDING && !CONFIG->workDir.empty()) {
        auto file = CONFIG->workDir + url.getUrl();
        if (std::filesystem::is_regular_file(file)) {
            printf("file %s", file.c_str());
            onProcHandleFile2(file, conn, stream);
        }
    }

    // http2 默认控制器
    if (stream->status == net::http::HttpHandleStatus::HANDING) {
        CONFIG->otherController(stream->req, stream->resp);
        auto len = stream->resp.getBody().getReadableSize();
        stream->resp.set("content-length", std::to_string(len));
        // response2Http2(stream->resp);
        writeHeader(conn, stream);
        if (len) {
            writeData(conn, stream);
        }
        stream->status = net::http::HttpHandleStatus::OK;
        // 流生存周期结束
        httpConn->streamMap.erase(stream->id);
    }
}

void sese::service::v2::HttpService::onProcHandleFile2(const std::string &path, sese::service::TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);

    if (stream->req.getType() != net::http::RequestType::Get) {
        stream->status = net::http::HttpHandleStatus::HANDING;
        return;
    }

    auto pos = path.find_last_of('.');
    if (pos != std::string::npos) {
        auto rawType = path.substr(pos + 1);
        auto iterator = net::http::HttpUtil::contentTypeMap.find(rawType);
        if (iterator != net::http::HttpUtil::contentTypeMap.end()) {
            stream->contentType = iterator->second;
        }
    }
    stream->resp.set("content-type", stream->contentType);

    auto lastModified = std::filesystem::last_write_time(path);
    uint64_t time = to_time_t(lastModified) * 1000 * 1000;
    stream->resp.set("last-modified", text::DateTimeFormatter::format(DateTime(time, 0), TIME_GREENWICH_MEAN_PATTERN));
    stream->fileSize = std::filesystem::file_size(path);
    stream->ranges = net::http::Range::parse(stream->req.get("Range", ""), stream->fileSize);

    stream->file = FileStream::create(path, "rb");
    if (stream->file == nullptr) {
        stream->resp.setCode(500);
        writeHeader(conn, stream);
        stream->status = net::http::HttpHandleStatus::OK;
        httpConn->streamMap.erase(stream->id);
        return;
    }
    // 完整文件传输
    if (stream->ranges.empty()) {
        stream->contentLength = stream->fileSize;
        stream->ranges.emplace_back(0, stream->fileSize);
        stream->rangeIterator = stream->ranges.begin();
        stream->resp.setCode(200);
        stream->resp.set("content-length", std::to_string(stream->contentLength));
        writeHeader(conn, stream);
        stream->status = net::http::HttpHandleStatus::FILE;
        return;
    }

    // 多区块传输
    stream->resp.setCode(206);
    stream->rangeIterator = stream->ranges.begin();
    if (stream->ranges.size() > 1) {
        for (auto &item: stream->ranges) {
            stream->contentLength += 12 +
                                     strlen(HTTPD_BOUNDARY) +
                                     strlen("content-type: ") +
                                     stream->contentType.length() +
                                     strlen("content-range: ") +
                                     item.toString(stream->fileSize).length() +
                                     item.len;
        }
        stream->contentLength += 8 + strlen(HTTPD_BOUNDARY);

        stream->resp.set("content-length", std::to_string(stream->contentLength));
        stream->resp.set("content-type", std::string("multipart/byteranges; boundary=") + HTTPD_BOUNDARY);
        writeHeader(conn, stream);
        stream->status = net::http::HttpHandleStatus::FILE;
    }
    // 单区块传输
    else {
        stream->contentLength = stream->rangeIterator->len;
        stream->resp.set("content-length", std::to_string(stream->rangeIterator->len));
        stream->resp.set("content-range", stream->rangeIterator->toString(stream->fileSize));
        writeHeader(conn, stream);
        stream->status = net::http::HttpHandleStatus::FILE;
    }
}

void sese::service::v2::HttpService::requestFromHttp2(net::http::Request &request) noexcept {
    request.setVersion(net::http::HttpVersion::VERSION_2);
    request.setUrl(request.get(":path", "/"));
    auto method = request.get(":method", "get");
    if (strcmpDoNotCase(method.c_str(), "get")) {
        request.setType(net::http::RequestType::Get);
    } else if (strcmpDoNotCase(method.c_str(), "options")) {
        request.setType(net::http::RequestType::Options);
    } else if (strcmpDoNotCase(method.c_str(), "post")) {
        request.setType(net::http::RequestType::Post);
    } else if (strcmpDoNotCase(method.c_str(), "head")) {
        request.setType(net::http::RequestType::Head);
    } else if (strcmpDoNotCase(method.c_str(), "put")) {
        request.setType(net::http::RequestType::Put);
    } else if (strcmpDoNotCase(method.c_str(), "delete")) {
        request.setType(net::http::RequestType::Delete);
    } else if (strcmpDoNotCase(method.c_str(), "trace")) {
        request.setType(net::http::RequestType::Trace);
    } else if (strcmpDoNotCase(method.c_str(), "connect")) {
        request.setType(net::http::RequestType::Connect);
    } else {
        request.setType(net::http::RequestType::Another);
    }
}

void sese::service::v2::HttpService::response2Http2(net::http::Response &response) noexcept {
    response.set(":status", std::to_string(response.getCode()));
}

void sese::service::v2::HttpService::writeFrame(uint8_t *buffer, const net::http::Http2FrameInfo &frame) noexcept {
    auto len = ToBigEndian32(frame.length);
    auto ident = ToBigEndian32(frame.ident);

    memcpy(buffer + 0, ((const char *) &len) + 1, 3);
    memcpy(buffer + 3, &frame.type, 1);
    memcpy(buffer + 4, &frame.flags, 1);
    memcpy(buffer + 5, &ident, 4);
}

void sese::service::v2::HttpService::writeHeader(sese::service::TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);

    net::http::Header onceHeader;
    net::http::Header indexedHeader;

    net::http::Header statusHeader{{":status", std::to_string(stream->resp.getCode())}};
    net::http::Header nullHeader{};

    // 排除非索引字段
    for (auto &item: stream->resp) {
        if (CONFIG->excludeIndexedHeader.count(item.first)) {
            onceHeader.set(item.first, item.second);
        } else {
            indexedHeader.set(item.first, item.second);
        }
    }

    // 此处为了确保 :status 留在首行，否则一些客户端将会视为错误
    ByteBuilder headerBuffer;
    auto headerSize = net::http::HPackUtil::encode(
            &headerBuffer,
            httpConn->dynamicTable2,
            nullHeader,
            statusHeader
    );

    headerSize += net::http::HPackUtil::encode(
            &headerBuffer,
            httpConn->dynamicTable2,
            onceHeader,
            indexedHeader
    );

    bool first = true;
    size_t targetFrameSize = httpConn->maxFrameSize;
    net::http::Http2FrameInfo frame{};
    frame.ident = stream->id;
    while (true) {
        auto len = headerSize >= targetFrameSize ? targetFrameSize : headerSize;
        if (len == 0) {
            break;
        } else if (len < targetFrameSize) {
            frame.flags = net::http::FRAME_FLAG_END_HEADERS;
        }

        if (first) {
            frame.type = net::http::FRAME_TYPE_HEADERS;
            first = false;
        } else {
            frame.type = net::http::FRAME_TYPE_CONTINUATION;
        }
        frame.length = (uint32_t) len;
        wrapper->writeFrame(frame);
        streamMove(&conn->buffer2write, &headerBuffer, len);
        headerSize -= len;
    }
}

void sese::service::v2::HttpService::writeData(sese::service::TcpConnection *conn, const Http2Stream::Ptr &stream) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);

    size_t bodySize = stream->resp.getBody().getReadableSize();
    size_t targetFrameSize = httpConn->maxFrameSize;
    net::http::Http2FrameInfo frame{};
    frame.ident = stream->id;
    while (true) {
        auto len = bodySize >= targetFrameSize ? targetFrameSize : bodySize;
        if (len == 0) {
            break;
        } else if (len < targetFrameSize) {
            frame.flags = net::http::FRAME_FLAG_END_STREAM;
        }
        frame.length = (uint32_t) len;
        wrapper->writeFrame(frame);
        streamMove(&conn->buffer2write, &stream->resp.getBody(), len);
        bodySize -= len;
    }
}

void sese::service::v2::HttpService::onSettingsFrame(sese::service::TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);

    char buffer[6];
    auto ident = (uint16_t *) &buffer[0];
    auto value = (uint32_t *) &buffer[2];

    size_t length = 0;

    while (conn->buffer2read.read(buffer, 6) == 6) {
        *ident = FromBigEndian16(*ident);
        *value = FromBigEndian32(*value);

        switch (*ident) {
            case net::http::SETTINGS_HEADER_TABLE_SIZE:
                httpConn->dynamicTable1.resize(*value);
                break;
            case net::http::SETTINGS_MAX_CONCURRENT_STREAMS:
                httpConn->maxConcurrentStream = *value;
                break;
            case net::http::SETTINGS_MAX_FRAME_SIZE:
                httpConn->maxFrameSize = *value;
                break;
            case net::http::SETTINGS_ENABLE_PUSH:
                httpConn->enablePush = *value;
                break;
            case net::http::SETTINGS_MAX_HEADER_LIST_SIZE:
                httpConn->maxHeaderListSize = *value;
            case net::http::SETTINGS_INITIAL_WINDOW_SIZE:
                httpConn->windowSize = *value;
            default:
                // 按照标准忽略该设置
                break;
        }

        length += 6;
        if (length == frame.length) {
            break;
        }
    }
}

void sese::service::v2::HttpService::onWindowUpdate(sese::service::TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    // 不做控制，读取负载
    wrapper->buffer2read.trunc(4);
}

void sese::service::v2::HttpService::onHeaderFrame(sese::service::TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    Http2Stream::Ptr stream;
    auto iterator = httpConn->streamMap.find(frame.ident);
    if (iterator == httpConn->streamMap.end()) {
        stream = std::make_shared<Http2Stream>();
        stream->id = frame.ident;
        stream->req.setVersion(net::http::HttpVersion::VERSION_2);
        stream->resp.setVersion(net::http::HttpVersion::VERSION_2);
        httpConn->streamMap[frame.ident] = stream;
    } else {
        stream = iterator->second;
    }

    stream->headerSize += frame.length;
    streamMove(&stream->req.getBody(), &wrapper->buffer2read, frame.length);

    if (frame.flags & net::http::FRAME_FLAG_END_HEADERS) {
        net::http::HPackUtil::decode(&stream->req.getBody(), stream->headerSize, httpConn->dynamicTable1, stream->req);
        requestFromHttp2(stream->req);
        stream->headerSize = 0;
        stream->req.getBody().freeCapacity();
        auto contentLength = stream->req.get("content-length", "0");
        if (contentLength == "0") {
            onProcDispatch2(conn, stream);
        }
    }
}

void sese::service::v2::HttpService::onDataFrame(sese::service::TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);
    Http2Stream::Ptr stream;
    auto iterator = httpConn->streamMap.find(frame.ident);
    if (iterator == httpConn->streamMap.end()) {
        stream = std::make_shared<Http2Stream>();
        stream->id = frame.ident;
        stream->req.setVersion(net::http::HttpVersion::VERSION_2);
        stream->resp.setVersion(net::http::HttpVersion::VERSION_2);
        httpConn->streamMap[frame.ident] = stream;
    } else {
        stream = iterator->second;
    }

    streamMove(&stream->req.getBody(), &wrapper->buffer2read, frame.length);

    if (frame.flags & net::http::FRAME_FLAG_END_STREAM) {
        onProcDispatch2(conn, stream);
    }
}

void sese::service::v2::HttpService::onResetFrame(sese::service::TcpConnection *conn, const net::http::Http2FrameInfo &frame) noexcept {
    auto wrapper = (HttpConnectionWrapper *) conn;
    auto httpConn = std::dynamic_pointer_cast<Http2Connection>(wrapper->conn);

    auto len = frame.length;
    conn->buffer2read.trunc(len);
    auto iterator = httpConn->streamMap.find(frame.ident);
    if (iterator != httpConn->streamMap.end()) {
        iterator->second->reset = true;
    }
}