#include <sese/service/WebsocketService.h>
#include <sese/service/BalanceLoader.h>

#include <random>

sese::net::IPv4Address::Ptr createAddress() {
    std::random_device device;
    auto engine = std::default_random_engine(device());
    std::uniform_int_distribution<uint16_t> dis(1025, 65535);
    auto port = dis(engine);
    printf("select port %d\n", (int) port);
    return sese::net::IPv4Address::create("0.0.0.0", 8080);
}

int main() {
    sese::service::WebsocketConfig config;
    config.servName = "Server for Test";
    config.upgradePath = "/chat";
    config.workDir = PROJECT_PATH;
    config.keepalive = 30;


    sese::service::BalanceLoader service;
    service.setThreads(4);
    service.setAddress(createAddress());
    service.init<sese::service::WebsocketService>([&config]() -> auto {
        return new sese::service::WebsocketService(&config);
    });
    service.start();
    getchar();
}