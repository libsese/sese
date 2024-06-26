#define SESE_C_LIKE_FORMAT

#include <sese/security/SSLContextBuilder.h>
#include <sese/net/AddressPool.h>
#include "sese/io/OutputUtil.h"
#include <sese/util/Random.h>
#include <sese/thread/Thread.h>
#include <sese/record/Marco.h>

#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(TestSSL, Auth) {
    auto context = sese::security::SSLContextBuilder::SSL4Server();
    ASSERT_NE(context, nullptr);
    ASSERT_NE(context->getContext(), nullptr);

    ASSERT_TRUE(context->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt"));
    ASSERT_TRUE(context->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem"));
    ASSERT_TRUE(context->authPrivateKey());
}

TEST(TestSSL, Client) {
    auto address = sese::net::IPv4AddressPool::lookup("microsoft.com");
    ASSERT_NE(address, nullptr);
    address->setPort(443);

    auto context = sese::security::SSLContextBuilder::SSL4Client();
    auto client = context->newSocketPtr(sese::net::Socket::Family::IPv4, IPPROTO_IP);
    ASSERT_EQ(client->connect(address), 0);

    *client << "GET / HTTP/1.1\r\n"
               "Host: microsoft.com\r\n"
               "\r\n";

    char buffer[1024]{};
    client->read(buffer, sizeof(buffer));
    // SESE_INFO("%s", buffer);

    client->shutdown(sese::net::Socket::ShutdownMode::BOTH);
    client->close();
}

TEST(TestSSL, Server) {
    auto port = sese::net::createRandomPort();
    auto address = sese::net::IPv4Address::localhost((uint16_t) port);
    auto serv_ctx = sese::security::SSLContextBuilder::SSL4Server();
    serv_ctx->importCertFile(PROJECT_PATH "/sese/test/Data/test-ca.crt");
    serv_ctx->importPrivateKeyFile(PROJECT_PATH "/sese/test/Data/test-key.pem");
    ASSERT_TRUE(serv_ctx->authPrivateKey());

    auto server = sese::security::SecuritySocket(serv_ctx, sese::net::Socket::Family::IPv4, IPPROTO_IP);
    ASSERT_EQ(server.bind(address), 0);
    server.listen(SERVER_MAX_CONNECTION);

    auto th = sese::Thread(
            [&server] {
                SESE_INFO("waiting accept");
                auto socket = server.accept();
                if (socket == nullptr) {
                    SESE_ERROR("failed to accept");
                    FAIL();
                }
                SESE_INFO("accepted");
                *socket << "Hello, Client";
                socket->close();
                SESE_INFO("close");
            },
            "SSLServer"
    );
    th.start();

    auto client_ctx = sese::security::SSLContextBuilder::SSL4Client();
    auto client = sese::security::SecuritySocket(client_ctx, sese::net::Socket::Family::IPv4, IPPROTO_IP);
    if (client.connect(address)) {
        SESE_ERROR("failed to connect");
        FAIL();
    }
    SESE_INFO("connected");
    char buffer[32]{};
    client.read(buffer, sizeof(buffer));
    client.close();
    SESE_INFO("Recv Message: %s", buffer);

    th.join();
    server.close();
}