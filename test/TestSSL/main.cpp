#include <sese/security/SSLContextBuilder.h>
#include <sese/Test.h>
#include <sese/record/LogHelper.h>
#include "openssl/err.h"

using sese::Initializer;
using sese::record::LogHelper;
using sese::Socket;
using sese::IPAddress;
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

    auto ip = IPAddress::create("110.242.68.66", 443);
    auto rt = client->connect(ip);
    assert(log, rt == 0, -3);

    auto len = client->write("GET /index.html HTTP/1.0\r\nHost: baidu.com\r\n\r\n", 55);
    assert(log, len == 55, -4);

    char buffer[1024]{};
    len = client->read(buffer, 1024);
    log.debug("buffer len = %d", len);
    puts(buffer);

    ERR_print_errors_fp(stdout);

    return 0;
}