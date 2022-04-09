#include "net/Socket.h"
#include "thread/Thread.h"
#include "Util.h"

#define FILTER_TEST_SOCKET "fSOCKET"

using sese::IPv4Address;
using sese::IPv6Address;
using sese::Socket;
using sese::Thread;

void IPv4ServerProc() {
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Server starting")
    auto server = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    auto address = IPv4Address::create("127.0.0.1", 7891);
    server->bind(address);
    server->listen(5);
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Server loading finished")
    auto client = server->accept();
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Accept client from %s", client->getAddress() ? client->getAddress()->getAddress().c_str() : "UNKNOWN ADDRESS")
    client->shutdown(Socket::ShutdownMode::Both);
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Shutdown connection")
    client->close();
    server->close();
}

void IPv6ServerProc() {
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Server starting")
    auto server = std::make_shared<Socket>(Socket::Family::IPv6, Socket::Type::TCP);
    auto address = IPv6Address::create("::1", 7891);
    server->bind(address);
    server->listen(5);
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Server loading finished")
    auto client = server->accept();
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Accept client from %s", client->getAddress() ? client->getAddress()->getAddress().c_str() : "UNKNOWN ADDRESS")
    client->shutdown(Socket::ShutdownMode::Both);
    ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Shutdown connection")
    client->close();
    server->close();
}

int main() {
    {
        ROOT_INFO(FILTER_TEST_SOCKET, "############################################################")
        sese::Thread threadIPv4Server(IPv4ServerProc, "ServerThreadIPv4");
        threadIPv4Server.start();
        sese::sleep(1);
        ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Client starting")
        auto client = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
        auto address = IPv4Address::create("127.0.0.1", 7891);
        client->connect(address);
        ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Client connected")
        client->shutdown(Socket::ShutdownMode::Both);
        ROOT_INFO(FILTER_TEST_SOCKET, "IPv4: Shutdown connection")
        client->close();
        threadIPv4Server.join();
    }
    ROOT_INFO(FILTER_TEST_SOCKET, "############################################################");
    {
        sese::Thread threadIPv6Server(IPv6ServerProc, "ServerThreadIPv6");
        threadIPv6Server.start();
        sese::sleep(1);
        auto client = std::make_shared<Socket>(Socket::Family::IPv6, Socket::Type::TCP);
        auto address = IPv6Address::create("::1", 7891);
        auto i = client->connect(address);
        ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Client connected")
        client->shutdown(Socket::ShutdownMode::Both);
        ROOT_INFO(FILTER_TEST_SOCKET, "IPv6: Shutdown connection")
        threadIPv6Server.join();
        ROOT_INFO(FILTER_TEST_SOCKET, "############################################################")
    }
    return 0;
}