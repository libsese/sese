#include "sese/net/Socket.h"
#include "RpcService_V1.h"
#include "sese/config/json/JsonUtil.h"
#include "sese/net/rpc/Marco.h"

#include "openssl/ssl.h"

sese::service::v1::RpcService::RpcService(const security::SSLContext::Ptr &context) noexcept {
    this->context = context;
}

void sese::service::v1::RpcService::setFunction(const std::string &name, const sese::service::v1::RpcService::Func &func) noexcept {
    this->funcs[name] = func;
}

sese::service::v1::RpcService::~RpcService() noexcept {
    for (decltype(auto) item: buffers) {
        delete item.second; // GCOVR_EXCL_LINE
    }
}

void sese::service::v1::RpcService::onAccept(int fd) {
    SSL *clientSSL = nullptr;

    // 一般不会失败
    // GCOVR_EXCL_START
    if (sese::net::Socket::setNonblocking(fd)) {
        sese::net::Socket::close(fd);
        return;
    }
    // GCOVR_EXCL_STOP

    if (context) {
        clientSSL = SSL_new((SSL_CTX *) context->getContext());
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

    this->buffers[fd] = new sese::io::ByteBuilder(2048);
    this->createEvent(fd, EVENT_READ, clientSSL);
}

void sese::service::v1::RpcService::onRead(sese::event::BaseEvent *event) {
    // done: 将 sese-event 更新至 0.1.4 可删除该判断
    // if (event->fd == listenFd) return;

    int64_t len = 0;
    auto buffer = this->buffers[event->fd];
    char buf[1024];
    while (true) {
        auto l = read(event->fd, buf, 1024, event->data);
        if (l <= 0) {
            if (errno == ENOTCONN) {
                // 断开连接
                if (context) {
                    SSL_free((SSL *) event->data);
                }
                buffers.erase(event->fd);
                delete buffer; // GCOVR_EXCL_LINE
                sese::net::Socket::close(event->fd);
                this->freeEvent(event);
                break;
            } else {
                // 无数据可读，触发子函数处理
                if (len && onHandle(buffer)) {
                    // 准备进入写模式，返回响应
                    event->events &= ~EVENT_READ; // 删除读事件
                    event->events |= EVENT_WRITE; // 添加写事件
                    this->setEvent(event);
                    break;
                } else {
                    // 处理错误，断开连接
                    buffers.erase(event->fd);
                    if (context) {
                        SSL_free((SSL *) event->data);
                    }
                    delete buffer; // GCOVR_EXCL_LINE
                    sese::net::Socket::close(event->fd);
                    this->freeEvent(event);
                    break;
                }
            }
        } else {
            len += l;
            buffer->write(buf, l);
        }
    }
}

void sese::service::v1::RpcService::onWrite(sese::event::BaseEvent *event) {
    // done: 将 sese-event 更新至 0.1.4 可删除该判断
    // if (event->fd == listenFd) return;

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
                if (context) {
                    SSL_free((SSL *) event->data);
                }
                buffers.erase(event->fd);
                delete buffer; // GCOVR_EXCL_LINE
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
        if (context) {
            SSL_free((SSL *) event->data);
        }
        buffers.erase(event->fd);
        delete buffer; // GCOVR_EXCL_LINE
        sese::net::Socket::close(event->fd);
        this->freeEvent(event);
    }
}

void sese::service::v1::RpcService::onClose(sese::event::BaseEvent *event) {
    // if (context) {
    //     SSL_free((SSL *) event->data);
    // }
    // auto buffer = buffers[event->fd];
    // buffers.erase(event->fd);
    // delete buffer; // GCOVR_EXCL_LINE
    // sese::net::Socket::close(event->fd);
    // this->freeEvent(event);
}

#define BuiltinSetExitCode(code) exit->setDataAs<int64_t>(code)

bool sese::service::v1::RpcService::onHandle(sese::io::ByteBuilder *builder) {
    auto object = json::JsonUtil::deserialize(builder, 5);
    if (object == nullptr) {
        return false;
    }

    auto result = std::make_shared<json::ObjectData>();

    auto exit = std::make_shared<json::BasicData>();
    BuiltinSetExitCode(SESE_RPC_CODE_SUCCEED);
    result->set(SESE_RPC_TAG_EXIT_CODE, exit);

    auto version = std::make_shared<json::BasicData>();
    version->setDataAs<std::string>(SESE_RPC_VERSION_0_1);
    result->set(SESE_RPC_TAG_VERSION, version);

    // 1.版本确定
    std::string ver;
    auto verData = object->getDataAs<json::BasicData>(SESE_RPC_TAG_VERSION);
    if (nullptr == verData) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
        return true;
    } else {
        ver = verData->getDataAs<std::string>(SESE_RPC_VALUE_UNDEF);
    }

    if (SESE_RPC_VERSION_0_1 != ver) {
        BuiltinSetExitCode(SESE_RPC_CODE_NONSUPPORT_VERSION);
        json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
        return true;
    }

    // 2.获取并检查远程调用名称
    std::string name;
    auto nameData = object->getDataAs<json::BasicData>(SESE_RPC_TAG_NAME);
    if (nullptr == nameData) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
        return true;
    } else {
        name = nameData->getDataAs<std::string>(SESE_RPC_VALUE_UNDEF);
    }

    auto iterator = funcs.find(name);
    if (iterator == funcs.end()) {
        BuiltinSetExitCode(SESE_RPC_CODE_NO_EXIST_FUNC);
        json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
        return true;
    }

    // 3.获取并检查用户过程
    auto func = iterator->second;
    auto args = object->getDataAs<json::ObjectData>(SESE_RPC_TAG_ARGS);
    if (!args) {
        BuiltinSetExitCode(SESE_RPC_CODE_MISSING_REQUIRED_FIELDS);
        json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
        return true;
    }

    // 4.执行用户过程
    func(args, result);

    // 5.序列化
    json::JsonUtil::serialize(result.get(), builder); // GCOVR_EXCL_LINE
    return true;
}

int64_t sese::service::v1::RpcService::read(int fd, void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_read((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::read(fd, buffer, len, 0);
    }
}

int64_t sese::service::v1::RpcService::write(int fd, const void *buffer, size_t len, void *ssl) noexcept {
    if (ssl) {
        return SSL_write((SSL *) ssl, buffer, (int) len);
    } else {
        return sese::net::Socket::write(fd, buffer, len, 0);
    }
}
