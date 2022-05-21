#include <sese/net/IPv6Address.h>
#include <sese/net/http/QueryString.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/record/LogHelper.h>
#include <sese/net/Socket.h>
#include <sese/Test.h>
#include <sese/thread/Thread.h>
#include <sese/Util.h>

#define PORT 7892

using sese::IPv4Address;
using sese::LogHelper;
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
    std::string url = "/index.html?val0=888&val1=&val2=1";
    helper.info("url: %s", url.c_str());
    auto queryString = QueryString();
    queryString.parse(url);

    helper.info("uri: %s", queryString.getUri().c_str());
    for (const auto &item: queryString) {
        helper.info("%s: %s", item.first.c_str(), item.second.c_str());
    }

    queryString.set("val1", "999");
    helper.info("toString: %s", queryString.toString().c_str());
}

void testHttpServer() {
    auto localAddress = IPv4Address::create("127.0.0.1", PORT);
    auto serverSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    Test::assert(helper, 0 == serverSocket->bind(localAddress), -1);
    serverSocket->listen(5);

    auto clientSocket = serverSocket->accept();

    auto requestHeader = std::make_shared<RequestHeader>();
    HttpUtil::recvRequest(clientSocket, requestHeader);

    for (const auto &item: *requestHeader) {
        helper.info("request %s: %s", item.first.c_str(), item.second.c_str());
    }

    auto responseHeader = sese::make_shared_from_list<ResponseHeader, ResponseHeader::KeyValueType>(
            {{"Location", "https://github.com/SHIINASAMA/sese"},
             {"Connection", "close"}}
    );
    responseHeader->setCode(302);
    HttpUtil::sendResponse(clientSocket, responseHeader);

    clientSocket->shutdown(Socket::ShutdownMode::Both);
    clientSocket->close();
    serverSocket->close();
}

void testHttpClient() {
    auto remoteAddress = IPv4Address::create("127.0.0.1", PORT);
    auto clientSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);

    Test::assert(helper, 0 == clientSocket->connect(remoteAddress), -1);
    auto requestHeader = std::make_shared<RequestHeader>();
    requestHeader->setType(RequestType::Get);
    requestHeader->setUrl("/");
    requestHeader->set("Connection", "close");
    HttpUtil::sendRequest(clientSocket, requestHeader);

    auto responseHeader = std::make_shared<ResponseHeader>();
    HttpUtil::recvResponse(clientSocket, responseHeader);

    for (const auto &item: *responseHeader) {
        helper.info("response %s: %s", item.first.c_str(), item.second.c_str());
    }

    clientSocket->shutdown(Socket::ShutdownMode::Both);
    clientSocket->close();
}

int main() {
    testQueryString();

    Thread thread(testHttpServer, "Server Thread");
    thread.start();
    sese::sleep(1);
    testHttpClient();
    thread.join();

    return 0;
}