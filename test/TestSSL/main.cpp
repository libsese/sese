#include <sese/security/SSLContextBuilder.h>
#include <sese/record/LogHelper.h>
#include <sese/net/AddressPool.h>
#include <sese/Test.h>

using sese::Initializer;
using sese::record::LogHelper;
using sese::Socket;
using sese::IPv4AddressPool;
using sese::security::SecuritySocket;
using sese::security::SSLContext;
using sese::security::SSLContextBuilder;

int main() {
    Initializer::getInitializer();
    LogHelper log("TestSSL");

    auto context = SSLContextBuilder::SSL4Client();
    assert(log, context != nullptr, -1);

    auto client = context->newSocketPtr(sese::Socket::Family::IPv4, 0);
    assert(log, client != nullptr, -2);


    auto ip = IPv4AddressPool::lookup("www.baidu.com");
    ip->setPort(443);
    auto rt = client->connect(ip);
    assert(log, rt == 0, -3);

    auto len = client->write("GET /index.html HTTP/1.0\r\nHost: baidu.com\r\n\r\n", 55);
    assert(log, len == 55, -4);

    char buffer[1025]{};
    len = client->read(buffer, 1024);
    log.debug("buffer len = %d", len);
    puts(buffer);
    return 0;
}