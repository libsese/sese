#include "sese/net/Socket.h"
#include "sese/record/LogHelper.h"
#include "sese/thread/Thread.h"
#include "sese/util/Util.h"

using sese::IPv4Address;
using sese::IPv6Address;
using sese::Socket;
using sese::Socket;
using sese::Thread;

sese::record::LogHelper helper("fSOCKET"); // NOLINT

void IPv4ServerProc() {
    helper.info("IPv4: Server starting");
    auto server = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    auto address = IPv4Address::create("127.0.0.1", 7891);
    server->bind(address);
    server->listen(5);
    helper.info("IPv4: Server loading finished");
    auto client = server->accept();
    helper.info("IPv4: Accept client from %s", client->getAddress() ? client->getAddress()->getAddress().c_str() : "UNKNOWN ADDRESS");
    client->shutdown(Socket::ShutdownMode::Both);
    helper.info("IPv4: Shutdown connection");
    client->close();
    server->close();
}

void IPv6ServerProc() {
    helper.info("IPv6: Server starting");
    auto server = std::make_shared<Socket>(Socket::Family::IPv6, Socket::Type::TCP);
    auto address = IPv6Address::create("::1", 7891);
    server->bind(address);
    server->listen(5);
    helper.info("IPv6: Server loading finished");
    auto client = server->accept();
    helper.info("IPv6: Accept client from %s", client->getAddress() ? client->getAddress()->getAddress().c_str() : "UNKNOWN ADDRESS");
    client->shutdown(Socket::ShutdownMode::Both);
    helper.info("IPv6: Shutdown connection");
    client->close();
    server->close();
}

int main() {
    {
        helper.info("############################################################");
        sese::Thread threadIPv4Server(IPv4ServerProc, "ServerThreadIPv4");
        threadIPv4Server.start();
        sese::sleep(1);
        helper.info("IPv4: Client starting");
        auto client = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
        auto address = IPv4Address::create("127.0.0.1", 7891);
        client->connect(address);
        helper.info("IPv4: Client connected");
        client->shutdown(Socket::ShutdownMode::Both);
        helper.info("IPv4: Shutdown connection");
        client->close();
        threadIPv4Server.join();
    }
    helper.info("############################################################");
    {
        sese::Thread threadIPv6Server(IPv6ServerProc, "ServerThreadIPv6");
        threadIPv6Server.start();
        sese::sleep(1);
        helper.info("IPv6: Client starting");
        auto client = std::make_shared<Socket>(Socket::Family::IPv6, Socket::Type::TCP);
        auto address = IPv6Address::create("::1", 7891);
        auto i = client->connect(address);
        helper.info("IPv6: Client connected");
        client->shutdown(Socket::ShutdownMode::Both);
        helper.info("IPv6: Shutdown connection");
        threadIPv6Server.join();
        helper.info("############################################################");
    }
    return 0;
}