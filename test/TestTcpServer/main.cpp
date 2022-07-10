#include <sese/net/TcpServer.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::TcpServer;
using sese::IOContext;
using sese::IPv4Address;
using sese::LogHelper;
using sese::Test;
using sese::Thread;
using sese::Socket;

LogHelper helper("fTCP_SERVER"); // NOLINT

int main() {
    auto address = IPv4Address::create("127.0.0.1", 8080);

    auto server = TcpServer::create(address);
    assert(helper, nullptr != server, -1)

    // 服务器线程
    Thread thread([&server](){
        server->loopWith([&helper = helper](IOContext *ioContext) {
            helper.info("Client address: %s", ioContext->getClientAddress()->getAddress().c_str());
            char buffer[1024]{};
            ioContext->read(buffer, 1024);
            helper.info("request is \'%s\'", buffer);
            memcpy(buffer, "HTTP/1.1 200 OK\r\n\r\n", 19); // NOLINT
            ioContext->write(buffer, 19);
            ioContext->close();
        });
    }, "TcpServer");
    thread.start();

    // 客户端模拟
    char buffer[1024] {"GET / HTTP/1.1\r\n\r\n"};
    Socket socket(Socket::Family::IPv4, Socket::Type::TCP);
    assert(helper, -1 != socket.connect(address), -2);
    assert(helper, -1 != socket.write((const void *)buffer, 1024), -3);
    helper.info("request sent");
    assert(helper, -1 != socket.read(buffer, 1024), -4);
    helper.info("response is \'%s\'", buffer);
    socket.close();

    server->shutdown();
    thread.join();
    helper.info("server shutdown");
    return 0;
}