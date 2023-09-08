#include <sese/service/WebsocketService.h>
#include <sese/service/BalanceLoader.h>
#include <sese/system/Process.h>
#include <sese/record/Marco.h>
#include <sese/util/OutputUtil.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

class MyEvent : public sese::net::ws::WebsocketEvent {
public:
    void onMessage(sese::service::WebsocketService *service, sese::net::ws::WebsocketSession *session) noexcept override {
        char buffer[1024]{};
        auto len = session->buffer.read(buffer, 1024);
        SESE_INFO("recv message from client %d: %s", session->fd, buffer);

        session->buffer.freeCapacity();
        session->buffer << "recv your message: ";
        // session->buffer.write("recv your message: ", 19);
        session->buffer.write(buffer, len);
        service->doWriteMessage(session);
    }
    void onBinary(sese::service::WebsocketService *service, sese::net::ws::WebsocketSession *session) noexcept override {
        SESE_INFO("recv binary from client %d", session->fd);
    }
    void onClose(sese::service::WebsocketService *service, sese::net::ws::WebsocketSession *session) noexcept override {
        SESE_INFO("client %d do close", session->fd);
    }
    void onTimeout(sese::service::WebsocketService *service, sese::net::ws::WebsocketSession *session) noexcept override {
        SESE_INFO("client %d has timed out", session->fd);
    }
};

GTEST_TEST(TestWebsocketService, _0) {
    auto addr = createAddress();

    sese::service::WebsocketConfig config;
    config.servName = "Server for Test";
    config.upgradePath = "/chat";
    config.keepalive = 30;

    MyEvent event;

    sese::service::BalanceLoader service;
    service.setThreads(2);
    service.setAddress(addr);
    service.init<sese::service::WebsocketService>([&]() -> auto {
        return new sese::service::WebsocketService(&config, &event);
    });
    service.start();
    ASSERT_TRUE(service.isStarted());

    std::string cmd = PY_EXECUTABLE " " PROJECT_PATH "/scripts/do_websocket.py ";
    cmd.append(std::to_string(addr->getPort()));
    auto process = sese::system::Process::create(cmd.c_str());
    if (process == nullptr) {
        service.stop();
        FAIL() << "failed to create process";
    }

    EXPECT_EQ(process->wait(), 0);
    SUCCEED();
    service.stop();
}