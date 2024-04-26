// #include "sese/service/Service.h"
#include "sese/net/Socket.h"
#include "sese/service/SystemBalanceLoader.h"
#include "sese/service/UserBalanceLoader.h"
#include "sese/service/TimerableService_V1.h"
#include "sese/service/TimerableService_V2.h"
#include "sese/record/Marco.h"
#include "gtest/gtest.h"

#include <random>

#define printf SESE_INFO

using namespace std::chrono_literals;

class MyService final : public sese::event::EventLoop {
public:
    ~MyService() override {
        printf("total socket into: %d", num);
    }

    void onAccept(int fd) override {
        num += 1;
        sese::net::Socket::close(fd);
    }

protected:
    int num = 0;
};

static sese::net::IPAddress::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

TEST(TestService, SystemBalanceLoader) {
    auto addr = createAddress();

    sese::service::SystemBalanceLoader service;
    service.setThreads(4);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<MyService>());

    service.start();
    ASSERT_TRUE(service.isStarted());

    std::vector<sese::net::Socket> socket_vector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        sese::net::Socket::setNonblocking(true);
        socket_vector.emplace_back(s);
    }
    for (decltype(auto) s: socket_vector) {
        s.connect(addr);
    }
    std::this_thread::sleep_for(300ms);
    for (decltype(auto) s: socket_vector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
    service.stop();
}

TEST(TestService, UserBalanceLoader) {
    auto addr = createAddress();

    sese::service::UserBalanceLoader service;
    service.setThreads(3);
    service.setAddress(addr);
    ASSERT_TRUE(service.init<MyService>());

    service.start();
    ASSERT_TRUE(service.isStarted());

    std::vector<sese::net::Socket> socket_vector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        sese::net::Socket::setNonblocking(true);
        socket_vector.emplace_back(s);
    }
    for (decltype(auto) s: socket_vector) {
        s.connect(addr);
    }
    std::this_thread::sleep_for(500ms);
    for (decltype(auto) s: socket_vector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
    service.stop();
}

class MyTimerableServiceV1 : public sese::service::v1::TimerableService {
public:
    void onAccept(int fd) override {
        printf("fd %d connect", fd);
        if (0 == sese::net::Socket::setNonblocking(static_cast<sese::socket_t>(fd))) {
            auto event = createEvent(fd, EVENT_READ, nullptr);
            createTimeoutEvent(fd, event, 3);
        } else {
            sese::net::Socket::close(fd);
        }
    }

    void onRead(sese::event::BaseEvent *event) override {
        auto timeout_event = getTimeoutEventByFd(event->fd);
        // timeoutEvent will not be nullptr
        cancelTimeoutEvent(timeout_event);
        char buffer[1024]{};
        sese::net::Socket::read(timeout_event->fd, buffer, 1024, 0);
        puts(buffer);
        setEvent(event);
        setTimeoutEvent(timeout_event, 3);
    }

    void onTimeout(sese::service::v1::TimeoutEvent *timeout_event) override {
        printf("fd %d close", timeout_event->fd);
        sese::net::Socket::close(timeout_event->fd);
        auto event = static_cast<sese::event::Event *>(timeout_event->data);
        freeEvent(event);
    }
};

TEST(TestService, TimerableService) {
    auto addr = createAddress();

    sese::service::UserBalanceLoader service;
    service.setThreads(3);
    service.setAddress(addr);
    service.setAcceptTimeout(500);
    service.setDispatchTimeout(500);
    ASSERT_TRUE(service.init<MyTimerableServiceV1>());

    service.start();
    ASSERT_TRUE(service.isStarted());

    std::vector<sese::net::Socket> socket_vector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        sese::net::Socket::setNonblocking(true);
        socket_vector.emplace_back(s);
    }
    for (decltype(auto) s: socket_vector) {
        s.connect(addr);
    }
    socket_vector[4].write("Hello", 5);
    std::this_thread::sleep_for(5s);
    for (decltype(auto) s: socket_vector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
    service.stop();
}

class MyTimerableServiceV2 : public sese::service::v2::TimerableService {
public:
    void onAccept(int fd) override {
        printf("fd %d connect", fd);
        if (0 == sese::net::Socket::setNonblocking(static_cast<sese::socket_t>(fd))) {
            auto event = createEvent(fd, EVENT_READ, nullptr);
            auto timeout = setTimeoutEvent(3, nullptr);
            timeout->data = event;
            event->data = timeout;
        } else {
            sese::net::Socket::close(fd);
        }
    }

    void onRead(sese::event::BaseEvent *event) override {
        auto timeout_event = static_cast<sese::service::v2::TimeoutEvent *>(event->data);
        // timeoutEvent will not be nullptr
        cancelTimeoutEvent(timeout_event);
        timeout_event = nullptr;
        // timeoutEvent has been delete
        char buffer[1024]{};
        sese::net::Socket::read(event->fd, buffer, 1024, 0);
        puts(buffer);

        timeout_event = setTimeoutEvent(3, nullptr);
        setEvent(event);
        event->data = timeout_event;
        timeout_event->data = event;
    }

    void onTimeout(sese::service::v2::TimeoutEvent *timeout_event) override {
        auto event = static_cast<sese::event::Event *>(timeout_event->data);
        printf("fd %d close", event->fd);
        sese::net::Socket::close(event->fd);
        freeEvent(event);
    }
};

TEST(TestService, TimerableService_V2) {
    auto addr = createAddress();

    sese::service::UserBalanceLoader service;
    service.setThreads(3);
    service.setAddress(addr);
    service.setAcceptTimeout(500);
    service.setDispatchTimeout(500);
    ASSERT_TRUE(service.init<MyTimerableServiceV2>());

    service.start();
    ASSERT_TRUE(service.isStarted());

    std::vector<sese::net::Socket> socket_vector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        sese::net::Socket::setNonblocking(true);
        socket_vector.emplace_back(s);
    }
    for (decltype(auto) s: socket_vector) {
        s.connect(addr);
    }
    socket_vector[4].write("Hello", 5);
    std::this_thread::sleep_for(5s);
    for (decltype(auto) s: socket_vector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
}

#include <sese/service/http/HttpService_V3.h>
#include <sese/security/SSLContextBuilder.h>

TEST(TestHttpService, HttpHttpServiceImpl_V3) {
    auto ssl = sese::security::SSLContextBuilder::SSL4Server();
    ssl->importCertFile(PROJECT_PATH "/test/Data/test-ca.crt");
    ssl->importPrivateKeyFile(PROJECT_PATH "/test/Data/test-key.pem");

    auto serv = sese::service::http::v3::HttpService::create();
    serv->setName("HttpServiceImpl_V3")
            .setAddress(sese::net::IPv4Address::localhost(9956))
            .setSSLContext(ssl)
            .setKeepalive(60);

    ASSERT_TRUE(serv->startup()) << serv->getLastError();
    getchar();
    serv->shutdown();
}