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
    class UserBalanceLoader;
    class MasterEventLoop;
}// namespace sese::service

/// 用户均衡负载器，此负载器适用于全部平台
class API sese::service::UserBalanceLoader {
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
    /// \tparam Service 需要启动的服务
    /// \return 是否初始化成功
    template<class Service>
    bool init() noexcept;

    /// 初始化均衡器资源
    /// \tparam Service 需要启动的服务
    /// \param creator Service 创建函数，创建成功返回实例指针，否则应该返回空表示创建失败
    /// \return 是否初始化成功
    template<class Service>
    bool init(std::function<Service *()> creator) noexcept;

    /// 启动当前负载器和服务
    void start() noexcept;

    /// 关闭当前负载器并卸载服务
    void stop() noexcept;

protected:
    void master() noexcept;

    void slave(
            sese::event::EventLoop *eventLoop,
            std::queue<socket_t> *masterQueue,
            std::queue<socket_t> *slaveQueue,
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
    std::queue<socket_t> *masterSocketQueueArray{nullptr};
    std::queue<socket_t> *slaveSocketQueueArray{nullptr};
    std::mutex *mutexArray{nullptr};
};

class sese::service::MasterEventLoop final : public sese::event::EventLoop {
public:
    void onAccept(int fd) override;

    std::queue<socket_t> socketQueue;
};

template<class Service>
bool sese::service::UserBalanceLoader::init() noexcept {
    return sese::service::UserBalanceLoader::init<Service>([]() -> Service * { return new Service; });
}

template<class Service>
bool sese::service::UserBalanceLoader::init(std::function<Service *()> creator) noexcept {
    if (address == nullptr) return false;

    socket = new net::Socket(
            address->getRawAddress()->sa_family == AF_INET ? net::Socket::Family::IPv4 : net::Socket::Family::IPv6,
            net::Socket::Type::TCP
    );
    if (-1 == socket->getRawSocket()) {
        return false;
    }

    if (!socket->setNonblocking(true)) {
        goto freeSocket;
    }

    if (0 != socket->bind(address)) {
        goto freeSocket;
    }

    if (0 != socket->listen(32)) {
        goto freeSocket;
    }

    masterEventLoop = new MasterEventLoop;
    masterEventLoop->setListenFd((int) socket->getRawSocket());
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
    masterSocketQueueArray = new std::queue<socket_t>[threads];
    slaveSocketQueueArray = new std::queue<socket_t>[threads];
    mutexArray = new std::mutex[threads];

    return true;

freeEvent:
    for (decltype(auto) eventLoop: eventLoopVector) {
        delete eventLoop;
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