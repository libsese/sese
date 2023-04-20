#include "sese/net/Socket.h"
#include "sese/net/http/V2HttpServerOption.h"
#include "sese/util/Random.h"
#include "gtest/gtest.h"

#include <chrono>

using namespace sese;

auto makeRandomPortAddr() {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024) + 1024);
    printf("select port %d\n", port);
    auto addr = sese::IPv4Address::localhost();
    addr->setPort(port);
    return addr;
}

TEST(TestHttp, _0) {
    auto addr = makeRandomPortAddr();
    ASSERT_TRUE(addr != nullptr);

    net::v2::http::HttpServerOption option(10);
    option.address = addr;
    auto serv = net::v2::Server::create(&option);
    serv->start();

    char buf0[]{"GET / HTTP/1.1\r\n"
                "connect: keep-alive\r\n\r\n"};
    char buf1[256]{};
    int64_t writeSize;

    sese::Socket client(sese::Socket::Family::IPv4, sese::Socket::Type::TCP);
    if (client.connect(addr)) {
        EXPECT_TRUE(false);
        goto shutdown;
    }

    for (int i = 0; i < 5; ++i) {
        writeSize = client.write(buf0, sizeof(buf0) - 1);
        if (writeSize <= 0) {
            EXPECT_TRUE(false);
            goto close;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        client.read(buf1, sizeof(buf1));
        printf("%s\n", buf1);
        memset(buf1, 0, sizeof(buf1));
    }

close:
    client.shutdown(Socket::ShutdownMode::Both);
    client.close();
    std::this_thread::sleep_for(std::chrono::seconds(1));
shutdown:
    serv->shutdown();
}