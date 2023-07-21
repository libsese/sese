#include <sese/net/Socket.h>
#include <sese/net/AddressPool.h>
#include <sese/record/Marco.h>
#include <sese/util/Random.h>
#include <sese/util/OutputUtil.h>

#include <gtest/gtest.h>

GTEST_TEST(TestSocket, Client) {
    auto address = sese::net::IPv4AddressPool::lookup("bing.com");
    GTEST_ASSERT_NE(address, nullptr);
    address->setPort(80);

    auto client = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP, IPPROTO_IP);
    GTEST_ASSERT_EQ(client.connect(address), 0);

    client << "GET / HTTP/1.1\r\n"
              "Host: bing.com\r\n"
              "\r\n";

    char buffer[1024]{};
    client.read(buffer, sizeof(buffer));
    SESE_INFO("%s", buffer);

    client.shutdown(sese::net::Socket::ShutdownMode::Both);
    client.close();
}

GTEST_TEST(TestSocket, Server) {
    auto port = (sese::Random::next() % (65535 - 1024)) + 1024;
    auto address = sese::net::IPv4Address::localhost((uint16_t) port);
    auto server = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP, IPPROTO_IP);
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
            "Server"
    );
    th.start();

    auto client = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::TCP, IPPROTO_IP);
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

GTEST_TEST(TestSocket, Server_IPv6) {
    auto port = (sese::Random::next() % (65535 - 1024)) + 1024;
    auto address = sese::net::IPv6Address::localhost((uint16_t) port);
    auto server = sese::net::Socket(sese::net::Socket::Family::IPv6, sese::net::Socket::Type::TCP, IPPROTO_IP);
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
            "Server_IPv6"
    );
    th.start();

    auto client = sese::net::Socket(sese::net::Socket::Family::IPv6, sese::net::Socket::Type::TCP, IPPROTO_IP);
    if (client.connect(address)) {
        SESE_ERROR("failed to connect");
        FAIL();
    }
    SESE_INFO("connected");
    char buffer[32]{};
    client.peek(buffer, sizeof(buffer));
    client.trunc(sizeof(buffer));
    client.close();
    SESE_INFO("Recv Message: %s", buffer);

    th.join();
    server.close();
}

GTEST_TEST(TestSocket, Server_UDP) {
    auto port = (sese::Random::next() % (65535 - 1024)) + 1024;
    auto address = sese::net::IPv4Address::localhost((uint16_t) port);
    auto server = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::UDP, IPPROTO_IP);
    ASSERT_EQ(server.bind(address), 0);

    auto th = sese::Thread(
            [&server] {
                SESE_INFO("waiting recv");
                char buffer[1024]{};
                auto clientAddress = std::make_shared<sese::net::IPv4Address>();
                server.recv(buffer, sizeof(buffer), clientAddress, 0);
                SESE_INFO("Recv Address: %s:%d", clientAddress->getAddress().c_str(), clientAddress->getPort());
                SESE_INFO("Recv Message: %s", buffer);
                SESE_INFO("close");
            },
            "Server_UDP"
    );
    th.start();

    auto client = sese::net::Socket(sese::net::Socket::Family::IPv4, sese::net::Socket::Type::UDP, IPPROTO_IP);
    char buffer[] = "Hello, Server";
    client.send(buffer, sizeof(buffer) - 1, address, 0);

    th.join();
    server.close();
}

GTEST_TEST(TestSocket, NativeAPI) {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024)) + 1024;

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = ToBigEndian16(port);
    address.sin_addr.s_addr = ToBigEndian32(0x7f000001);

    auto socket = sese::net::Socket::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    GTEST_ASSERT_NE(socket, -1);
    GTEST_ASSERT_EQ(sese::net::Socket::bind(socket, (const sockaddr *) &address, sizeof(address)), 0);
    GTEST_ASSERT_EQ(sese::net::Socket::listen(socket, SERVER_MAX_CONNECTION), 0);

    auto th = sese::Thread(
            [socket] {
                SESE_INFO("waiting accept");
                auto client = sese::net::Socket::accept(socket);
                SESE_INFO("accepted");
                sese::net::Socket::write(client, "Hello", 5, 0);
                sese::net::Socket::close(client);
                SESE_INFO("closed");
            },
            "Server_NativeAPI"
    );
    th.start();

    auto client = sese::net::Socket::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    GTEST_ASSERT_NE(client, -1);
    SESE_INFO("connecting");
    GTEST_ASSERT_EQ(sese::net::Socket::connect(client, (const sockaddr *) &address, sizeof(address)), 0);
    SESE_INFO("connected");

    char buffer[32]{};
    sese::net::Socket::read(client, buffer, sizeof(buffer), 0);
    sese::net::Socket::close(client);
    SESE_INFO("Recv Message: %s", buffer);

    th.join();
    sese::net::Socket::close(socket);
}