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

    server->loopWith([&server](const HttpServiceContext::Ptr &context) {
        decltype(auto) request = context->getRequest();
        auto url = request->getUrl();
        helper.info("request url: %s", url.c_str());

        decltype(auto) response = context->getResponse();
        response->setCode(200);
        if (url == "/shutdown") {
            response->set("Content-length", "23");
            context->flush();
            context->write("Server is shutting down", 23);
            server->shutdown();
        } else {
            response->set("Content-length", "17");
            context->flush();
            context->write("Hello Sese Httpd!", 17);
        }
    });

    return 0;
}