#include <sese/net/IPv6Address.h>
#include <sese/net/http/QueryString.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/record/LogHelper.h>
#include <sese/net/Socket.h>
#include "sese/util/Test.h"
#include <sese/thread/Thread.h>
#include "sese/util/Util.h"

#define PORT 7892

using sese::IPv4Address;
using sese::record::LogHelper;
using sese::Socket;
using sese::Test;
using sese::Thread;
using sese::http::HttpUtil;
using sese::http::QueryString;
using sese::http::RequestHeader;
using sese::http::RequestType;
using sese::http::ResponseHeader;

LogHelper helper("fHTTP");//NOLINT

void testQueryString() {
    std::string query = "?val0=888&val1=&val2=1";
    helper.info("query: %s", query.c_str());
    auto queryString = QueryString(query);

    for (const auto &item: queryString) {
        helper.info("%s: %s", item.first.c_str(), item.second.c_str());
    }

    queryString.set("val1", "999");
    helper.info("toString: %s", queryString.toString().c_str());
}

void testHttpServer() {
    auto localAddress = IPv4Address::create("127.0.0.1", PORT);
    auto serverSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    assert(helper, 0 == serverSocket->bind(localAddress), -1);
    serverSocket->listen(5);

    auto clientSocket = serverSocket->accept();

    auto requestHeader = std::make_unique<RequestHeader>();
    HttpUtil::recvRequest(clientSocket.get(), requestHeader.get());

    for (const auto &item: *requestHeader) {
        helper.info("request %s: %s", item.first.c_str(), item.second.c_str());
    }

    auto responseHeader = sese::makeUniqueFromList<ResponseHeader, ResponseHeader::KeyValueType>(
            {{"Location", "https://github.com/SHIINASAMA/sese"},
             {"Connection", "close"}}
    );
    responseHeader->setCode(302);
    HttpUtil::sendResponse(clientSocket.get(), responseHeader.get());

    clientSocket->shutdown(Socket::ShutdownMode::Both);
    clientSocket->close();
    serverSocket->close();
}

void testHttpClient() {
    auto remoteAddress = IPv4Address::create("127.0.0.1", PORT);
    auto clientSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);

    assert(helper, 0 == clientSocket->connect(remoteAddress), -1);
    auto requestHeader = std::make_unique<RequestHeader>();
    requestHeader->setType(RequestType::Get);
    requestHeader->setUrl("/");
    requestHeader->set("Connection", "close");
    HttpUtil::sendRequest(clientSocket.get(), requestHeader.get());

    auto responseHeader = std::make_unique<ResponseHeader>();
    HttpUtil::recvResponse(clientSocket.get(), responseHeader.get());

    for (const auto &item: *responseHeader) {
        helper.info("response %s: %s", item.first.c_str(), item.second.c_str());
    }

    clientSocket->shutdown(Socket::ShutdownMode::Both);
    clientSocket->close();
}

int main() {
    testQueryString();

//    Thread thread(testHttpServer, "Server Thread");
//    thread.start();
//    sese::sleep(1);
//    testHttpClient();
//    thread.join();

    return 0;
}