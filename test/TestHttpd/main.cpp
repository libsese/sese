#include <sese/net/http/HttpServer.h>
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"
#include "sese/util/Util.h"

using sese::IPv4Address;
using sese::record::LogHelper;
using sese::Thread;
using sese::http::HttpServer;
using sese::http::HttpServiceContext;

sese::record::LogHelper helper("HTTPD");//NOLINT

int main() {
    auto address = IPv4Address::create("0.0.0.0", 8080);
    auto server = HttpServer::create(address, 4);
    assert(helper, server != nullptr, -1);

    Thread serverThread([&server]() {
        server->loopWith([](const HttpServiceContext::Ptr &context) {
            decltype(auto) request = context->getRequest();
            auto url = request->getUrl();
            helper.info("request url: %s", url.c_str());

            decltype(auto) response = context->getResponse();
            response->setCode(200);
            response->set("Content-length", "17");
            // 此句是可以省略的
            // context->flush();
            context->write("Hello Sese Httpd!", 17);
            return;
        });
    });

    serverThread.start();
    // sese::sleep(5);
    getchar();
    server->shutdown();
    serverThread.join();

    return 0;
}