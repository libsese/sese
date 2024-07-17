/// \file SystemBalanceLoader.h
/// \brief 系统均衡负载器
/// \author kaoru
/// \version 0.1
/// \date 2023年6月3日

#pragma once

#include "sese/event/Event.h"
#include "sese/net/ReusableSocket.h"
#include "sese/thread/Thread.h"

#include <atomic>
#include <functional>
#include <mutex>

namespace sese::service {

/// \brief 系统均衡负载器（无用户调度的负载器）
/// \warning 此负载器仅在 Linux 上生效
/// \see sese::service::BalanceLoader
class  SystemBalanceLoader final {
public:
    ~SystemBalanceLoader() noexcept;

    /// 设置负载器使用线程数量
    /// \param th 线程数量
    void setThreads(size_t th) noexcept;

    /// 设置服务启动地址
    /// \param addr IP Address
    void setAddress(const net::IPAddress::Ptr &addr) noexcept { SystemBalanceLoader::address = addr; }

    /// 设置从线程派遣超时时间
    /// \param to 超时时间，单位毫秒
    void setAcceptTimeout(uint32_t to) noexcept { SystemBalanceLoader::timeout = to; }

    /// 设置从线程派遣超时时间
    /// \param to 超时时间，单位毫秒
    void setDispatchTimeout(uint32_t to) noexcept { SystemBalanceLoader::timeout = to; }

    /// 获取当前负载器状态
    /// \return 负载器状态状态
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

protected:
    std::atomic_bool _isStart{false};
    std::atomic_bool _isStop{false};

    uint32_t timeout = 100;
    size_t threads{2};
    std::vector<socket_t> socketVector;
    std::vector<event::EventLoop *> eventLoopVector;
    std::vector<sese::Thread::Ptr> threadVector;
    sese::net::IPAddress::Ptr address = sese::net::IPv4Address::localhost(8080);
};
} // namespace sese::service

template<class SERVICE>
bool sese::service::SystemBalanceLoader::init() noexcept {
    return sese::service::SystemBalanceLoader::init<SERVICE>([]() -> SERVICE * { return new SERVICE; });
}

// 此处测试代码不便于模拟
// GCOVR_EXCL_START

template<class SERVICE>
bool sese::service::SystemBalanceLoader::init(std::function<SERVICE *()> creator) noexcept {
    if (address == nullptr) return false;

    sese::net::ReusableSocket reusable_socket(address);
    for (size_t i = 0; i < threads; ++i) {
        auto sub_socket = reusable_socket.makeRawSocket();
        if (sub_socket == -1) {
            goto freeSocket;
        }
        if (0 != sese::net::Socket::setNonblocking(sub_socket)) {
            goto freeSocket;
        }
        if (0 != sese::net::Socket::listen(sub_socket, 32)) {
            goto freeSocket;
        }
        socketVector.emplace_back(sub_socket);
    }

    for (size_t i = 0; i < threads; ++i) {
        auto event = creator();
        if (event == nullptr) {
            goto freeEvent;
        }
        event->setListenFd(static_cast<int>(socketVector[i]));
        if (!event->init()) {
            delete event;
            goto freeEvent;
        } else {
            eventLoopVector.emplace_back(event);
        }
    }

    return true;

freeEvent:
    for (decltype(auto) event_loop: eventLoopVector) {
        delete event_loop;
    }
    eventLoopVector.clear();

freeSocket:
    for (decltype(auto) sub_socket: socketVector) {
        sese::net::Socket::close(sub_socket);
    }
    socketVector.clear();
    return false;
}

// GCOVR_EXCL_STOP