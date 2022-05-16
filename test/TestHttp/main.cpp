#include <sese/net/IPv6Address.h>
#include <sese/net/http/QueryString.h>
#include <sese/net/http/HttpUtil.h>
#include <sese/record/LogHelper.h>
#include <sese/net/Socket.h>
#include <sese/ByteBuilder.h>
#include <sese/Test.h>

using sese::IPv4Address;
using sese::LogHelper;
using sese::Socket;
using sese::http::HttpUtil;
using sese::http::QueryString;
using sese::http::RequestHeader;
using sese::http::ResponseHeader;
using sese::Test;

LogHelper helper("fHTTP"); //NOLINT

void testQueryString() {
    std::string rawUrl = "/index.html?val0=888&val1=&val2=1";
    helper.info("rawUrl: %s", rawUrl.c_str());
    auto queryString = QueryString();
    queryString.parse(rawUrl);

    helper.info("url: %s", queryString.getUrl().c_str());
    for (const auto &item: queryString) {
        helper.info("%s: %s", item.first.c_str(), item.second.c_str());
    }

    queryString.set("val1", "999");
    helper.info("toString: %s", queryString.toString().c_str());
}

void testHttpServer() {
    auto localAddress = IPv4Address::create("127.0.0.1", 7891);
    auto serverSocket = std::make_shared<Socket>(Socket::Family::IPv4, Socket::Type::TCP);
    Test::assert(helper, 0 == serverSocket->bind(localAddress), -1);
    serverSocket->listen(5);

    auto clientSocket = serverSocket->accept();

    auto requestHeader = std::make_shared<RequestHeader>();
    HttpUtil::recvRequest(clientSocket, requestHeader);

    auto responseHeader = std::make_shared<ResponseHeader>();
    responseHeader->setCode(302);
    responseHeader->set("Location", "https://github.com/SHIINASAMA/sese");
    HttpUtil::sendResponse(clientSocket, responseHeader);

    clientSocket->shutdown(Socket::ShutdownMode::Both);
    clientSocket->close();
    serverSocket->close();
}

void testHttpClient() {

}

int main() {
    testQueryString();
    testHttpServer();
    return 0;
}