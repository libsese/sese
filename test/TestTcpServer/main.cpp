#include <sese/net/TcpServer.h>

int main() {
    sese::TcpServer server;
    server.init(sese::IPv4Address::create("0.0.0.0", 8080));
    server.loopWith([](sese::IOContext * ioContext){
        auto count = ioContext->recv();
        ioContext->close();
        setbuf(stdout, nullptr);
        puts(ioContext->buffer);
        fflush(stdout);
    });
    return 0;
}