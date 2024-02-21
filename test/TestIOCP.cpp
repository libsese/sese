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

    void onAcceptCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onAcceptCompleted %d", ctx->getFd());
        postRead(ctx);
    }

    void onPreRead(sese::iocp::Context *ctx) override {
        SESE_INFO("onRreRead %d", ctx->getFd());
    }

    void onReadCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onReadCompleted %d", ctx->getFd());
        sese::streamMove(ctx, ctx, IOCP_WSABUF_SIZE);
        postWrite(ctx);
    }

    void onWriteCompleted(sese::iocp::Context *ctx) override {
        SESE_INFO("onWriteCompleted %d", ctx->getFd());
    }

    void onConnected(sese::iocp::Context *ctx) override {
        SESE_INFO("onConnected %d", ctx->getFd());
    }

    static void myDeleter(sese::iocp::Context *ctx) {
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
    servCtx->importCertFile(PROJECT_PATH "/test/Data/test-ca.crt");
    servCtx->importPrivateKeyFile(PROJECT_PATH "/test/Data/test-key.pem");
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