#include "sese/net/V2Server.h"
#include "sese/security/SSLContextBuilder.h"
#include "sese/security/SecuritySocket.h"
#include "sese/util/Random.h"
#include "sese/util/Util.h"
#include "gtest/gtest.h"

auto makeRandomPortAddr() {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024) + 1024);
    printf("select port %d\n", port);
    auto addr = sese::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

class RawServerOption : public sese::net::v2::ServerOption {
public:
    void onConnect(sese::net::v2::IOContext &context) noexcept override {
        socket_t ident = context.getIdent();
        printf("connecting %d\n", (int) ident);
    }

    void onHandle(sese::net::v2::IOContext &context) noexcept override {
        char buf[1024]{};
        auto readSize = context.read(buf, 1024);
        ASSERT_TRUE(readSize > 0);
        auto writeSize = context.write(buf, readSize);
        EXPECT_TRUE(writeSize == readSize);
        context.close();
    }

    void onClosing(sese::net::v2::IOContext &context) noexcept override {
        socket_t ident = context.getIdent();
        printf("closing %d\n", (int) ident);
    }
};

TEST(TestServer, RawServer) {
    auto addr = makeRandomPortAddr();
    ASSERT_TRUE(addr != nullptr);

    RawServerOption option;
    option.address = addr;
    option.isSSL = false;
    option.threads = 2;

    auto server = sese::net::v2::Server::create(&option);
    ASSERT_TRUE(server != nullptr);
    server->start();

    char buf0[]{"Hello!"};
    char buf1[16]{};
    int64_t writeSize, readSize;

    sese::Socket client(sese::Socket::Family::IPv4, sese::Socket::Type::TCP);
    if (client.connect(addr)) {
        EXPECT_TRUE(false);
        goto shutdown;
    }

    writeSize = client.write(buf0, sizeof(buf0));
    if (writeSize <= 0) {
        EXPECT_TRUE(false);
        goto close;
    }

    readSize = client.read(buf1, sizeof(buf1));
    EXPECT_TRUE(readSize == writeSize);
    EXPECT_TRUE(0 == strcmp(buf0, buf1));

close:
    client.shutdown(sese::Socket::ShutdownMode::Both);
    client.close();
shutdown:
    server->shutdown();
}

TEST(TestServer, RawSSLServer) {
    auto addr = makeRandomPortAddr();
    ASSERT_TRUE(addr != nullptr);

    auto servCtx = sese::security::SSLContextBuilder::SSL4Server();
    servCtx->importCertFile(PROJECT_PATH "/test/TestSecTcpServer/test-ca.crt");
    servCtx->importPrivateKey(PROJECT_PATH "/test/TestSecTcpServer/test-key.pem");

    RawServerOption option;
    option.address = addr;
    option.isSSL = true;
    option.sslContext = servCtx;
    option.threads = 2;

    auto server = sese::net::v2::Server::create(&option);
    ASSERT_TRUE(server != nullptr);
    server->start();

    char buf0[]{"Hello!"};
    char buf1[16]{};
    int64_t writeSize, readSize;

    auto clientCtx = sese::security::SSLContextBuilder::SSL4Client();
    sese::security::SecuritySocket client(clientCtx, sese::Socket::Family::IPv4, 0);
    if (client.connect(addr)) {
        EXPECT_TRUE(false);
        goto shutdown;
    }

    writeSize = client.write(buf0, sizeof(buf0));
    if (writeSize <= 0) {
        EXPECT_TRUE(false);
        goto close;
    }

    readSize = client.read(buf1, sizeof(buf1));
    EXPECT_TRUE(readSize == writeSize);
    EXPECT_TRUE(0 == strcmp(buf0, buf1));

close:
    client.shutdown(sese::Socket::ShutdownMode::Both);
    client.close();
shutdown:
    server->shutdown();
}