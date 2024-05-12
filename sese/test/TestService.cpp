// #include "sese/service/Service.h"
#include "sese/net/Socket.h"
#include "sese/net/http/RequestableFactory.h"
#include "sese/service/SystemBalanceLoader.h"
#include "sese/service/UserBalanceLoader.h"
#include "sese/service/TimerableService_V1.h"
#include "sese/service/TimerableService_V2.h"
#include "sese/service/http/HttpServer_V3.h"
#include "sese/security/SSLContextBuilder.h"
#include "sese/record/Marco.h"
#include "gtest/gtest.h"

#pragma region HttpServer_V3

#define ASSERT_NOT_NULL(x) ASSERT_TRUE(x != nullptr)

SESE_CTRL(MyController) {
    SESE_URL(get_info, RequestType::GET, "/get_info?{name}") {
        auto name = req.getQueryArg("name");
        resp.set("name", name);
    };
    SESE_URL(get_info_2, RequestType::GET, "/get_info_2?<name>") {
        auto name = req.get("name");
        resp.set("name", name);
    };
}

class TestHttpServerV3 : public testing::Test {
public:
    static std::unique_ptr<sese::service::http::v3::HttpServer> server;

    static void SetUpTestSuite() {
        using sese::service::http::v3::HttpServer;

        auto ssl = sese::security::SSLContextBuilder::UniqueSSL4Server();
        ssl->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt");
        ssl->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem");

        server = std::make_unique<HttpServer>();
        server->setKeepalive(60);
        server->setName("HttpServiceImpl_V3");
        server->regMountPoint("/www", PROJECT_PATH);
        server->regController<MyController>();
        server->regService(sese::net::IPv4Address::localhost(9090), std::move(ssl));
        server->regService(sese::net::IPv4Address::localhost(9091), nullptr);

        ASSERT_TRUE(server->startup());
    }

    static void TearDownTestSuite() {
        server->shutdown();
    }
};

std::unique_ptr<sese::service::http::v3::HttpServer> TestHttpServerV3::server;

TEST_F(TestHttpServerV3, OnceRequest_query) {
    using namespace sese::net::http;
    {
        auto client = RequestableFactory::createHttpRequest("https://127.0.0.1:9090/get_info?name=sese");
        ASSERT_NOT_NULL(client);
        ASSERT_TRUE(client->request()) << client->getLastError();
        EXPECT_EQ(client->getResponse()->getCode(), 200);
        for (auto &&[key, value]: *client->getResponse()) {
            SESE_INFO("%s: %s", key.c_str(), value.c_str());
        }
    }
    {
        auto client = RequestableFactory::createHttpRequest("http://127.0.0.1:9091/get_info");
        ASSERT_NOT_NULL(client);
        ASSERT_TRUE(client->request()) << client->getLastError();

        EXPECT_EQ(client->getResponse()->getCode(), 400);
        for (auto &&[key, value]: *client->getResponse()) {
            SESE_INFO("%s: %s", key.c_str(), value.c_str());
        }
    }
}

TEST_F(TestHttpServerV3, OnceRequest_header) {
    using namespace sese::net::http;
    {
        auto client = RequestableFactory::createHttpRequest("https://127.0.0.1:9090/get_info_2");
        ASSERT_NOT_NULL(client);
        ASSERT_TRUE(client->request()) << client->getLastError();
        EXPECT_EQ(client->getResponse()->getCode(), 400);
        for (auto &&[key, value]: *client->getResponse()) {
            SESE_INFO("%s: %s", key.c_str(), value.c_str());
        }
    }
    {
        auto client = RequestableFactory::createHttpRequest("http://127.0.0.1:9091/get_info_2");
        ASSERT_NOT_NULL(client);
        client->getRequest()->set("name", "kaoru");
        ASSERT_TRUE(client->request()) << client->getLastError();

        EXPECT_EQ(client->getResponse()->getCode(), 200);
        for (auto &&[key, value]: *client->getResponse()) {
            SESE_INFO("%s: %s", key.c_str(), value.c_str());
        }
    }
}

TEST_F(TestHttpServerV3, Keepalive) {
    using namespace sese::net::http;
    auto client = RequestableFactory::createHttpRequest("http://127.0.0.1:9091/get_info_2");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();
    EXPECT_EQ(client->getResponse()->getCode(), 400);
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
    client->getRequest()->set("name", "kaoru");
    ASSERT_TRUE(client->request()) << client->getLastError();
    EXPECT_EQ(client->getResponse()->getCode(), 200);
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

TEST_F(TestHttpServerV3, Keepalive_ssl) {
    using namespace sese::net::http;
    auto client = RequestableFactory::createHttpRequest("https://127.0.0.1:9090/get_info_2");
    ASSERT_NOT_NULL(client);
    ASSERT_TRUE(client->request()) << client->getLastError();
    EXPECT_EQ(client->getResponse()->getCode(), 400);
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
    client->getRequest()->set("name", "kaoru");
    ASSERT_TRUE(client->request()) << client->getLastError();
    EXPECT_EQ(client->getResponse()->getCode(), 200);
    for (auto &&[key, value]: *client->getResponse()) {
        SESE_INFO("%s: %s", key.c_str(), value.c_str());
    }
}

#pragma endregion

#pragma region BuiltinEventModel

#define printf SESE_INFO

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
    socket_vector[4].write("Hello", 5); // NOLINT
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
    socket_vector[4].write("Hello", 5); // NOLINT
    std::this_thread::sleep_for(5s);
    for (decltype(auto) s: socket_vector) {
        s.close();
    }

    std::this_thread::sleep_for(300ms);
}

#pragma endregion