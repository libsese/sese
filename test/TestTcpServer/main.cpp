#include <sese/net/TcpServer.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>
#include <sese/Util.h>

using sese::IOContext;
using sese::IPv4Address;
using sese::LogHelper;
using sese::Socket;
using sese::TcpServer;
using sese::Test;
using sese::Thread;

LogHelper helper("fTCP_SERVER");// NOLINT

int main() {
    auto address = IPv4Address::create("0.0.0.0", 8080);

    auto server = TcpServer::create(address, 4, 30);
    assert(helper, nullptr != server, -1);

    // 服务器线程
    Thread thread([&server]() {
        server->loopWith([&helper = helper](IOContext *ioContext) {
            char buffer[1024]{};
            ioContext->read(buffer, 1024);
            helper.info(buffer);
            ioContext->write("HTTP/1.1 200 OK\r\n"
                             "Host: www.sese.com:8080\r\n"
                             "Connection: Keep-alive\r\n"
                             "Content-Length: 0\r\n\r\n", 17 + 25 + 21 + 19 + 5);
        });
    },
                  "TcpServer");
    thread.start();
    sese::sleep(5);
    puts("WANT SHUTDOWN");
    server->shutdown();
    thread.join();
    return 0;
}