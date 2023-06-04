// #include "sese/service/Service.h"
#include "sese/service/SystemBalanceLoader.h"
#include "sese/service/UserBalanceLoader.h"
#include "gtest/gtest.h"

#include <random>

using namespace std::chrono_literals;

class MyService final : public sese::event::EventLoop {
public:
    ~MyService() override {
        printf("total socket into: %d\n", num);
    }

    void onAccept(int fd) override {
        num += 1;
        sese::net::Socket::close(fd);
    }

protected:
    int num = 0;
};

sese::net::IPAddress::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
    printf("select port %d\n", (int) port);
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

    std::vector<sese::net::Socket> socketVector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        s.setNonblocking(true);
        socketVector.emplace_back(s);
    }
    for (decltype(auto) s: socketVector) {
        s.connect(addr);
    }
    std::this_thread::sleep_for(500ms);
    for (decltype(auto) s: socketVector) {
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

    std::vector<sese::net::Socket> socketVector;
    for (int i = 0; i < 20; ++i) {
        auto s = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
        s.setNonblocking(true);
        socketVector.emplace_back(s);
    }
    for (decltype(auto) s: socketVector) {
        s.connect(addr);
    }
    std::this_thread::sleep_for(500ms);
    for (decltype(auto) s: socketVector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
    service.stop();
}