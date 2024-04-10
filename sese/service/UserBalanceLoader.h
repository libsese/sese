/// \file UserBalanceLoader.h
/// \brief 用户均衡负载器
/// \author kaoru
/// \version 0.1
/// \date 2023年6月5日

#pragma once

#include "sese/event/Event.h"
#include "sese/net/Socket.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <queue>
#include <mutex>

namespace sese::service {

class MasterEventLoop;

/// 用户均衡负载器，此负载器适用于全部平台
class API UserBalanceLoader {
public:
    ~UserBalanceLoader() noexcept;

    /// 设置负载器使用线程数量
    /// \param th 线程数量
    void setThreads(size_t th) noexcept;

    /// 设置服务启动地址
    /// \param addr IP Address
    void setAddress(const net::IPAddress::Ptr &addr) noexcept { UserBalanceLoader::address = addr; }

    /// 设置主监听线程超时时间
    /// \param to 超时时间，单位毫秒
    void setAcceptTimeout(uint32_t to) noexcept { UserBalanceLoader::acceptTimeout = to; }

    /// 设置从线程派遣超时时间
    /// \param to 超时时间，单位毫秒
    void setDispatchTimeout(uint32_t to) noexcept { UserBalanceLoader::dispatchTimeout = to; }

    /// 获取当前负载器状态
    /// \return 负载器状态
    [[nodiscard]] bool isStarted() const { return _isStart; }

    /// 初始化负载器资源
    /// \tparam SERVICE 需要启动的服务
    /// \return 是否初始化成功
    template<class SERVICE>
    bool init() noexcept;

    /// 初始化均衡器资源
    /// \tparam SERVICE 需要启动的服务
    /// \param creator Service 创建函数，创建成功返回实例指针，否则应该返回空表示创建失败
    /// \return 是否初始化成功
    template<class SERVICE>
    bool init(std::function<SERVICE *()> creator) noexcept;

    /// 启动当前负载器和服务
    void start() noexcept;

    /// 关闭当前负载器并卸载服务
    void stop() noexcept;

    void dispatchSocket(socket_t sock, void *data);

    void setOnDispatchedCallbackFunction(const std::function<void(int, sese::event::EventLoop *, void *)> &cb) { UserBalanceLoader::onDispatchedCallbackFunction = cb; }

protected:
    /// 套接字状态
    struct SocketStatus {
        socket_t fd{};
        void *data{};
    };

    void master() noexcept;

    void slave(
            sese::event::EventLoop *event_loop,
            std::queue<SocketStatus> *master_queue,
            std::queue<SocketStatus> *slave_queue,
            std::mutex *mutex
    ) noexcept;

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    uint32_t acceptTimeout = 100;
    uint32_t dispatchTimeout = 100;
    size_t threads{2};
    std::vector<sese::event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);

    sese::net::Socket *socket{nullptr};
    sese::service::MasterEventLoop *masterEventLoop{nullptr};
    sese::Thread::Ptr masterThread{nullptr};
    /// socket_t 交换队列
    std::queue<SocketStatus> *masterSocketQueueArray{nullptr};
    std::queue<SocketStatus> *slaveSocketQueueArray{nullptr};
    std::mutex *mutexArray{nullptr};

    std::function<void(int, sese::event::EventLoop *event_loop, void *)> onDispatchedCallbackFunction;
};

/// 用户均衡负载器主线程
class MasterEventLoop final : public sese::event::EventLoop {
public:
    void onAccept(int fd) override;

    std::queue<socket_t> socketQueue;
};
} // namespace sese::service

// 此处测试代码不便于模拟
// GCOVR_EXCL_START

template<class SERVICE>
bool sese::service::UserBalanceLoader::init() noexcept {
    return sese::service::UserBalanceLoader::init<SERVICE>([]() -> SERVICE * { return new SERVICE; });
}

template<class SERVICE>
bool sese::service::UserBalanceLoader::init(std::function<SERVICE *()> creator) noexcept {
    if (address == nullptr) return false;

    socket = new net::Socket(
            address->getRawAddress()->sa_family == AF_INET ? net::Socket::Family::IPv4 : net::Socket::Family::IPv6,
            net::Socket::Type::TCP
    );
    if (-1 == socket->getRawSocket()) {
        return false;
    }

    if (!socket->setNonblocking()) {
        goto freeSocket;
    }

    if (0 != socket->bind(address)) {
        goto freeSocket;
    }

    if (0 != socket->listen(32)) {
        goto freeSocket;
    }

    masterEventLoop = new MasterEventLoop;
    masterEventLoop->setListenFd(static_cast<int>(socket->getRawSocket()));
    if (!masterEventLoop->init()) {
        goto freeMaster;
    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = creator();
        if (event == nullptr) {
            goto freeEvent;
        }
        if (!event->init()) {
            delete event;
            goto freeEvent;
        } else {
            eventLoopVector.emplace_back(event);
        }
    }

    // 初始化交换队列
    masterSocketQueueArray = new std::queue<SocketStatus>[threads];
    slaveSocketQueueArray = new std::queue<SocketStatus>[threads];
    mutexArray = new std::mutex[threads];

    return true;

freeEvent:
    for (decltype(auto) event_loop: eventLoopVector) {
        delete event_loop;
    }
    eventLoopVector.clear();

freeMaster:
    delete masterEventLoop;
    masterEventLoop = nullptr;

freeSocket:
    socket->close();
    delete socket;
    socket = nullptr;
    return false;
}

// GCOVR_EXCL_STOP