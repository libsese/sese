#include <gtest/gtest.h>

#include <sese/service/iocp/IOCPServer.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/record/Marco.h>
#include <sese/io/OutputUtil.h>
#include <sese/util/Util.h>

class MyIOCPServer : public sese::iocp::IOCPServer {
public:
    MyIOCPServer() {
        setDeleteContextCallback(myDeleter);
    }

    void onAcceptCompleted(Context *ctx) override {
        SESE_INFO("onAcceptCompleted %d", ctx->getFd());
        postRead(ctx);
    }

    void onPreRead(Context *ctx) override {
        SESE_INFO("onRreRead %d", ctx->getFd());
    }

    void onReadCompleted(Context *ctx) override {
        SESE_INFO("onReadCompleted %d", ctx->getFd());
        sese::streamMove(ctx, ctx, IOCP_WSABUF_SIZE);
        postWrite(ctx);
    }

    void onWriteCompleted(Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", ctx->getFd());
    }

    void onConnected(Context *ctx) override {
        SESE_INFO("onConnected %d", ctx->getFd());
    }

    static void myDeleter(Context *ctx) {
        SESE_INFO("onDeleteCallback %d", ctx->getFd());
    }
};

TEST(TestIOCP, Server_0) {
    auto address = sese::net::IPv4Address::localhost(sese::net::createRandomPort());

    MyIOCPServer server;
    server.setAddress(address);
    server.setThreads(2);
    server.setServProtos("\x8http/1.1");
    ASSERT_TRUE(server.init());
    SESE_INFO("server listening on %d", address->getPort());

    sese::net::Socket socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP);
    ASSERT_EQ(socket.connect(address), 0);
    socket << "Hello World";

    char buffer[32]{};
    socket.read(buffer, sizeof(buffer));
    SESE_INFO("echo from server, %s", buffer);
    socket.close();

    server.shutdown();
}

TEST(TestIOCP, Server_1) {
    auto servCtx = sese::security::SSLContextBuilder::SSL4Server();
    servCtx->importCertFile(PROJECT_PATH "/gtest/Data/test-ca.crt");
    servCtx->importPrivateKeyFile(PROJECT_PATH "/gtest/Data/test-key.pem");
    ASSERT_TRUE(servCtx->authPrivateKey());

    auto address = sese::net::IPv4Address::localhost(sese::net::createRandomPort());

    MyIOCPServer server;
    server.setAddress(address);
    server.setServCtx(servCtx);
    server.setThreads(2);
    server.setServProtos("\x8http/1.1");
    ASSERT_TRUE(server.init());
    SESE_INFO("server listening on %d", address->getPort());

    auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
    auto socket = clientCtx->newSocketPtr(sese::net::Socket::Family::IPv4, 0);
    ASSERT_EQ(socket->connect(address), 0);
    *socket << "Hello World";

    char buffer[32]{};
    socket->read(buffer, sizeof(buffer));
    SESE_INFO("echo from server, %s", buffer);
    socket->close();

    server.shutdown();
}

class MyIOCPClient : public sese::iocp::IOCPServer {
public:
    MyIOCPClient() {
        setDeleteContextCallback(myDeleter);
    }

    void onPreRead(Context *ctx) override {
        SESE_INFO("onRreRead %d", ctx->getFd());
    }

    void onReadCompleted(Context *ctx) override {
        SESE_INFO("onReadCompleted %d", ctx->getFd());
        char buffer[1024] {};
        int64_t len = ctx->peek(buffer, 1);
        if (len == 0) {
            postRead(ctx);
            return;
        }

        while ((len = ctx->read(buffer, sizeof(buffer))) > 0) {
            SESE_RAW(buffer, len - 1);
        }
        postClose(ctx);
    }

    void onWriteCompleted(Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", ctx->getFd());
        postRead(ctx);
    }

    void onPreConnect(Context *ctx) override {
        SESE_INFO("onPreConnect %d", ctx->getFd());
    }

    void onConnected(Context *ctx) override {
        SESE_INFO("onConnected %d", ctx->getFd());
        auto buffer = "HTTP/1.1 / GET\r\n"
                      "Host: microsoft.com\r\n"
                      "\r\n";
        ctx->write(buffer, strlen(buffer));
        postWrite(ctx);
    }

    static void myDeleter(Context *ctx) {
        SESE_INFO("onDeleteCallback %d", ctx->getFd());
    }
};

TEST(TestIOCP, Client_0) {
    auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
    MyIOCPClient client;
    client.setThreads(2);
    ASSERT_TRUE(client.init());

    {
        auto address = sese::net::IPv4Address::lookUpAny("microsoft.com");
        auto ipAddress = std::reinterpret_pointer_cast<sese::net::IPv4Address>(address);
        auto sock1 = sese::net::Socket::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        ipAddress->setPort(80);
        client.postConnect(sock1, ipAddress, nullptr);
    }

    {
        auto address = sese::net::IPv4Address::lookUpAny("microsoft.com");
        auto ipAddress = std::reinterpret_pointer_cast<sese::net::IPv4Address>(address);
        auto sock2 = sese::net::Socket::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        ipAddress->setPort(443);
        client.postConnect(sock2, ipAddress, clientCtx);
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3s);
    client.shutdown();
    SESE_INFO("shutdown");
}