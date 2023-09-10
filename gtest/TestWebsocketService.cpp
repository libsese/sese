#include <sese/service/WebsocketService.h>
#include <sese/service/BalanceLoader.h>
#include <sese/system/Process.h>
#include <sese/record/Marco.h>
#include <sese/util/OutputUtil.h>
#include <sese/net/ws/WebsocketAuthenticator.h>

#include <random>

#include <gtest/gtest.h>

#define printf SESE_INFO

static sese::net::IPv4Address::Ptr createAddress() {
    auto port = sese::net::createRandomPort();
    printf("select port %d", (int) port);
    return sese::net::IPv4Address::create("127.0.0.1", port);
}

TEST(TestWebsocket, Auth_0) {
    auto key = "dGhlIHNhbXBsZSBub25jZQ==";
    auto result = sese::net::ws::WebsocketAuthenticator::toResult(key);
    EXPECT_EQ(std::strcmp("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", result.get()), 0);
}

TEST(TestWebsocket, Auth_1) {
    auto pair = sese::net::ws::WebsocketAuthenticator::generateKeyPair();
    EXPECT_TRUE(sese::net::ws::WebsocketAuthenticator::verify(pair.first.get(), pair.second.get()));
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

class MyEvent : public sese::net::ws::WebsocketEvent {
public:
    void onMessage(sese::service::WebsocketService *service, sese::net::ws::WebsocketSession *session) noexcept override {
        printf("on\n");
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

GTEST_TEST(TestWebsocketService, DISABLED_Server) {
    auto addr = createAddress();

    sese::service::WebsocketConfig config;
    config.servName = "Server for Test";
    config.upgradePath = "/chat";
    config.keepalive = 30;

    MyEvent event;

    sese::service::BalanceLoader service;
    service.setThreads(1);
    service.setAddress(addr);
    service.init<sese::service::WebsocketService>([&]() -> auto {
        return new sese::service::WebsocketService(&config, &event);
    });
    service.start();
    ASSERT_TRUE(service.isStarted());

    // std::string cmd = PY_EXECUTABLE " " PROJECT_PATH "/scripts/do_websocket.py ";
    // cmd.append(std::to_string(addr->getPort()));
    // auto process = sese::system::Process::create(cmd.c_str());
    // if (process == nullptr) {
    //     service.stop();
    //     FAIL() << "failed to create process";
    // }

    // EXPECT_EQ(process->wait(), 0);
    getchar();
    SUCCEED();
    service.stop();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif