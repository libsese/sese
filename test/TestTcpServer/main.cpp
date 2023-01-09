#include <sese/net/TcpServer.h>
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"
#include "sese/util/Util.h"

using sese::IOContext;
using sese::IPv4Address;
using sese::record::LogHelper;
using sese::Socket;
using sese::TcpServer;
using sese::Test;
using sese::Thread;

LogHelper helper("fTCP_SERVER");// NOLINT

int main() {
    auto address = IPv4Address::create("0.0.0.0", 8080);
#ifndef _WIN32
    auto socket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    assert(helper, socket->bind(address) == 0, -1);
    assert(helper, socket->listen(5) == 0, -1);
    auto server = TcpServer::create(socket, 4, 0);
    assert(helper, nullptr != server, -1);
#else
    auto server = TcpServer::create(address, 4, 0);
    assert(helper, nullptr != server, -1);
#endif

    // 服务器线程
    Thread thread([&server]() {
        server->loopWith([&helper = helper](IOContext *ioContext) {
            helper.info("recv request");
            // 此处仅演示可在 Windows 平台下未使用 SSL 的可优化选项
#ifdef WIN32
            char *buffer = ioContext->buffer;
#else
            char buffer[1024]{};
            ioContext->read(buffer, 1024);
#endif
            puts(buffer);
            ioContext->write("HTTP/1.1 200 OK\r\n"
                             "Host: www.sese.com:8080\r\n"
                             "Connection: Keep-alive\r\n"
                             "Content-Length: 0\r\n\r\n", 87);
            ioContext->close();
        });
    },
                  "TcpServer");
    thread.start();
    sese::sleep(5);
//    getchar();
    helper.info("WANT SHUTDOWN");
    server->shutdown();
    thread.join();
    return 0;
}