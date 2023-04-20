#include "sese/net/http/V2HttpServerOption.h"
#include "gtest/gtest.h"
#include "sese/util/Random.h"

using namespace sese;

auto makeRandomPortAddr() {
    auto port = (uint16_t) (sese::Random::next() % (65535 - 1024) + 1024);
    printf("select port %d\n", port);
    auto addr = sese::IPv4Address::any();
    addr->setPort(port);
    return addr;
}

//TEST(TestHttp, _0) {
int main() {
    auto addr = makeRandomPortAddr();
    addr->setPort(8080);
//    ASSERT_TRUE(addr != nullptr);

    net::v2::http::HttpServerOption option(1000);
    option.address = addr;
    auto serv = net::v2::Server::create(&option);
    serv->start();

    getchar();

    serv->shutdown();
}