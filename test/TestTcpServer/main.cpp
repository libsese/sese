#include <sese/net/TcpServer.h>

int main() {
    sese::TcpServer server;
    server.init(sese::IPv4Address::create("0.0.0.0", 8080));
    server.loopWith([](sese::IOContext * ioContext){
        auto count = ioContext->recv();
        memcpy(ioContext->buffer, "HTTP/1.1 200 OK\r\n\r\n", 19);
        ioContext->wsaBuf.len = 23;
        ioContext->send();
        ioContext->close();
    });
    return 0;
}