/// \file SystemBalanceLoader.h
/// \brief 系统均衡负载器
/// \author kaoru
/// \version 0.1
/// \date 2023年6月3日

#pragma once

#include "sese/event/Event.h"
#include "sese/net/ReusableSocket.h"
#include "sese/thread/Thread.h"

namespace sese::service {
    class SystemBalanceLoader;
}

/// 系统均衡负载器（无用户调度的负载器）
/// \warning 此负载器仅在 Linux 上生效
/// \see sese::service::BalanceLoader
class API sese::service::SystemBalanceLoader final {
public:
    ~SystemBalanceLoader() noexcept;

    /// 设置负载器使用线程数量
    /// \param th 线程数量
    void setThreads(size_t th) noexcept;

    /// 设置服务启动地址
    /// \param addr IP Address
    void setAddress(const net::IPAddress::Ptr &addr) { SystemBalanceLoader::address = addr; }

    /// 获取当前负载器状态
    /// \return 负载器状态状态
    [[nodiscard]] bool isStarted() const { return _isStart; }

    /// 初始化负载器资源
    /// \tparam Service 需要启动的服务
    /// \return 是否初始化成功
    template<class Service>
    bool init() noexcept;

    /// 启动当前负载器和服务
    void start() noexcept;

    /// 关闭当前负载器并卸载服务
    void stop() noexcept;

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    size_t threads{2};
    std::vector<socket_t> socketVector;
    std::vector<event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);
};

template<class Service>
bool sese::service::SystemBalanceLoader::init() noexcept {
    if (address == nullptr) return false;

    sese::net::ReusableSocket reusableSocket(address);
    for (size_t i = 0; i < threads; ++i) {
        auto subSocket = reusableSocket.makeRawSocket();
        if (subSocket == -1) {
            goto free_socket;
        }
        if (0 != sese::net::Socket::setNonblocking(subSocket)) {
            goto free_socket;
        }
        if (0 != sese::net::Socket::listen(subSocket, 32)) {
            goto free_socket;
        }
        socketVector.emplace_back(subSocket);

    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = new Service;
        event->setListenFd((int) socketVector[i]);
        if (!event->init()) {
            delete event;
            goto free_event;
        } else {
            eventLoopVector.emplace_back(event);
        }
    }

    return true;

free_event:
    for (decltype(auto) eventLoop: eventLoopVector) {
        delete eventLoop;
    }
    eventLoopVector.clear();

free_socket:
    for (decltype(auto) subSocket: socketVector) {
        sese::net::Socket::close(subSocket);
    }
    socketVector.clear();
    return false;
}
