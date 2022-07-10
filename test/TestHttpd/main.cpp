#include <sese/net/http/HttpServer.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::IPv4Address;
using sese::LogHelper;
using sese::http::HttpServer;
using sese::http::HttpServiceContext;

LogHelper helper("HTTPD");//NOLINT

int main() {
    auto address = IPv4Address::create("0.0.0.0", 8080);
    auto server = HttpServer::create(address);
    assert(helper, server != nullptr, -1);

    server->loopWith([](const HttpServiceContext::Ptr &context) {
        decltype(auto) request = context->getRequest();
        auto url = request->getUrl();
        helper.info("request url: %s", url.c_str());

        decltype(auto) response = context->getResponse();
        response->setCode(200);
        response->set("content-length","34");
        context->flush();

        context->write("<center>Hello Sese Httpd!</center>", 34);
    });

    getchar();
    server->shutdown();

    return 0;
}