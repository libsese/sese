#include "sese/net/ReusableSocket.h"
#include "sese/event/Event.h"
#include "sese/thread/Thread.h"
#include "gtest/gtest.h"

#include <atomic>
#include <random>

using namespace std::chrono_literals;

class MyEvent : public sese::event::EventLoop {
public:
    void onAccept(int fd) override {
        num++;
    }

    void start() {
        th = std::make_unique<sese::Thread>([this]() {
            while (run) {
                dispatch(100);
            }
        }, "MyEventThread");
        th->start();
    }

    void stop() {
        run = false;
        th->join();
    }

    int getNum() const { return num; }

private:
    std::atomic_int num{0};
    std::atomic_bool run{true};
    sese::Thread::Ptr th{nullptr};
};

sese::net::IPAddress::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
    printf("select port %d\n", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

TEST(TestReusableSocket, LoadBalancing) {
    auto addr = createAddress();
    sese::net::ReusableSocket reusableSocket(addr);

    auto sock1 = reusableSocket.makeRawSocket();
    auto sock2 = reusableSocket.makeRawSocket();

    ASSERT_NE(sock1, -1);
    ASSERT_NE(sock2, -1);

    ASSERT_EQ(sese::net::Socket::setNonblocking(sock1), 0);
    ASSERT_EQ(sese::net::Socket::setNonblocking(sock2), 0);

    sese::net::Socket::listen(sock1, 15);
    sese::net::Socket::listen(sock2, 15);

    MyEvent event1;
    event1.setListenFd((int) sock1);
    ASSERT_TRUE(event1.init());

    MyEvent event2;
    event2.setListenFd((int) sock2);
    ASSERT_TRUE(event2.init());

    event1.start();
    event2.start();

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

    event1.stop();
    event2.stop();

    sese::net::Socket::close(sock1);
    sese::net::Socket::close(sock2);

    printf("Socket1: %d\nSocket2: %d\n", event1.getNum(), event2.getNum());
}