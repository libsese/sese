//#include <fcntl.h>
//
//#include "sese/net/ReadableServer.h"
//
//int64_t sese::net::IOContext::read(void *dst, size_t length) {
//    return ::read(socket, dst, length);
//}
//
//int64_t sese::net::IOContext::write(const void *src, size_t length) const {
//    return ::write(socket, src, length);
//}
//
//void sese::net::IOContext::close() {
//    isClosed = true;
//    shutdown(socket, SHUT_RDWR);
//    ::close(socket);
//}
//
//sese::net::ReadableServer::Ptr sese::net::ReadableServer::create(const Handler &handler, size_t threads) noexcept {
//    auto epollFd = EpollCreate(0);
//    if (-1 == epollFd) {
//        return nullptr;
//    }
//
//    auto server = new ReadableServer;
//    server->ioContextPool = ObjectPool<sese::net::IOContext>::create();
//    server->workerThread = std::make_unique<Thread>([server]() { server->LinuxWorkerFunction(); }, "ReadableServer" + std::to_string(threads));
//    server->threadPool = std::make_unique<ThreadPool>("ReadableServer", threads);
//    server->handler = handler;
//    server->epollFd = epollFd;
//    server->workerThread->start();
//    return std::unique_ptr<ReadableServer>(server);
//}
//
//bool sese::net::ReadableServer::add(socket_t socket) noexcept {
//    auto opt = fcntl(socket, F_GETFL);
//    if (-1 == opt) {
//        return false;
//    }
//
//    if (-1 == fcntl(socket, F_SETFL, opt | O_NONBLOCK)) {
//        return false;
//    }
//
//    EpollEvent ev{};
//    ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
//    ev.data.fd = socket;
//    if (-1 == EpollCtl(epollFd, EPOLL_CTL_ADD, socket, &ev)) {
//        return false;
//    }
//
//    return true;
//}
//
//void sese::net::ReadableServer::shutdown() noexcept {
//    isShutdown = true;
//    workerThread->join();
//    threadPool->shutdown();
//    close(epollFd);
//}
//
//void sese::net::ReadableServer::LinuxWorkerFunction() noexcept {
//    int32_t nfds;
//    socket_t client;
//    while (true) {
//        if (isShutdown) break;
//
//        nfds = EpollWait(epollFd, events, MaxEvents, 0);
//        if (-1 == nfds) continue;
//        for (auto i = 0; i < nfds; ++i) {
//            if (events[i].events & EPOLLIN) {
//                // 缓冲区可读
//                if (events[i].events & EPOLLRDHUP) {
//                    // FIN 标识，断开连接，不做处理
//                    close(events[i].data.fd);
//                    continue;
//                }
//
//                client = events[i].data.fd;
//                auto ctx = ioContextPool->borrow();
//                ctx->socket = client;
//                threadPool->postTask([ctx, this]() {
//                    this->handler(ctx.get());
//
//                    if (!ctx->isClosed) {
//                        EpollEvent ev{};
//                        ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
//                        ev.data.fd = ctx->socket;
//                        EpollCtl(epollFd, EPOLL_CTL_MOD, ctx->socket, &ev);
//                    }
//                });
//            }
//        }
//    }
//}