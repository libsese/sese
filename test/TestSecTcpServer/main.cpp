#include "sese/security/SecurityTcpServer.h"
#include "sese/security/SSLContextBuilder.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"

int main() {
    sese::record::LogHelper helper("SEC_SERVER");

    auto ctx = sese::security::SSLContextBuilder::SSL4Server();
    ctx->importPrivateKey(PROJECT_PATH "/test/TestSecTcpServer/test-key.pem");
    ctx->importCertFile(PROJECT_PATH "/test/TestSecTcpServer/test-ca.crt");
    auto auth = ctx->authPrivateKey();
    assert(helper, auth, -1);

    auto ip = sese::IPAddress::create("0.0.0.0", 8080);
    auto server = sese::security::SecurityTcpServer::create(ip, 4, 10, ctx);
    assert(helper, server != nullptr, -2);

    sese::Thread th([&server, &helper](){
        server->loopWith([&helper](sese::security::IOContext *ctx){
            helper.debug("recv request");
            char buffer[1024]{};
            ctx->read(buffer, 1024);

            ctx->write("HTTP/1.1 200 OK\r\n"
                       "Host: www.sese.com:8080\r\n"
                       "Connection: Keep-alive\r\n"
                       "Content-Length: 5\r\n\r\n", 87);
            ctx->write("Hello", 5);
            ctx->close();
        });
    });
    th.start();
    getchar();
    server->shutdown();
    th.join();
    return 0;
}