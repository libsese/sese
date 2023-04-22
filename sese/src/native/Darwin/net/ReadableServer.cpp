//#include "sese/net/ReadableServer.h"
//
//#include <unistd.h>
//#include <fcntl.h>
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
//    auto kqueueFd = kqueue();
//    if (-1 == kqueueFd) {
//        return nullptr;
//    }
//
//    auto server = new ReadableServer;
//    server->ioContextPool = ObjectPool<sese::net::IOContext>::create();
//    server->workerThread = std::make_unique<Thread>([server]() { server->DarwinWorkerFunction(); }, "ReadableServer" + std::to_string(threads));
//    server->threadPool = std::make_unique<ThreadPool>("ReadableServer", threads);
//    server->handler = handler;
//    server->kqueueFd = kqueueFd;
//    server->workerThread->start();
//    return std::unique_ptr<ReadableServer>(server);
//}
//
//bool sese::net::ReadableServer::add(socket_t socket) noexcept {
//    int32_t opt = fcntl(socket, F_GETFL);
//    if (-1 == opt) {
//        return false;
//    }
//
//    if (-1 == fcntl(socket, F_SETFL, opt | O_NONBLOCK)) {
//        return false;
//    }
//
//    KEvent ev{};
//    EV_SET(&ev, socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
//    if (-1 ==  kevent(kqueueFd, &ev, 1, nullptr, 0, nullptr)) {
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
//    close(kqueueFd);
//}
//
//void sese::net::ReadableServer::DarwinWorkerFunction() noexcept {
//    struct timespec timeout {
//        1, 0
//    };
//
//    while (true) {
//        if (isShutdown) break;
//
//        int32_t nev = kevent(kqueueFd, nullptr, 0, events, MaxEvents, &timeout);
//        if (-1 == nev) continue;
//        for (auto n = 0; n < nev; ++n) {
//            if (events[n].filter == EVFILT_READ) {
//                if (events[n].flags & EV_EOF) {
//                    close(events[n].ident);
//                    continue;
//                }
//
//                socket_t client = events[n].ident;
//                auto ctx = ioContextPool->borrow();
//                ctx->socket = client;
//
//                threadPool->postTask([ctx, this]() {
//                    handler(ctx.get());
//
//                    if (!ctx->isClosed) {
//                        KEvent ev{};
//                        EV_SET(&ev, ctx->socket, EVFILT_READ, EV_ADD | EV_ONESHOT, 0, 0, nullptr);
//                        kevent(kqueueFd, &ev, 1, nullptr, 0, nullptr);
//                    }
//                });
//            }
//        }
//    }
//}