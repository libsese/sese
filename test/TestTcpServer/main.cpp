#include <sese/net/TcpServer.h>
#include <sese/record/LogHelper.h>

using sese::TcpServer;
using sese::IOContext;
using sese::IPv4Address;
using sese::LogHelper;

LogHelper helper("fTCP_SERVER"); // NOLINT

int main() {
    TcpServer server;
    server.init(IPv4Address::create("0.0.0.0", 8080));
    server.loopWith([&helper = helper](IOContext *ioContext) {
        helper.info("Client address: %s", ioContext->getClientAddress()->getAddress().c_str());
        char buffer[1024]{};
        ioContext->recv(buffer, 1024);
        memcpy(buffer, "HTTP/1.1 200 OK\r\n\r\n", 19);
        ioContext->send(buffer, 19);
        ioContext->close();
    });
    return 0;
}