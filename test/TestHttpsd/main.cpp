#include "sese/net/http/HttpServer.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"
#include "sese/util/Util.h"

int main() {
    sese::record::LogHelper log("HTTPS");

    sese::http::HttpsServer::SecurityOptions options;
    options.CA = PROJECT_PATH "/test/TestSecTcpServer/test-ca.crt";
    options.KEY = PROJECT_PATH "/test/TestSecTcpServer/test-key.pem";

    auto ip = sese::IPAddress::create("0.0.0.0", 8080);

    auto server = sese::http::HttpsServer::create(ip, 4, 10, options);
    assert(log, server != nullptr, -1);

    sese::Thread th([&server, &log]() {
        server->loopWith([&log](sese::http::HttpsServer::ServiceContext *context) {
            auto request = &context->request;
            auto url = request->getUrl();
            log.info("request url: %s", url.c_str());

            auto response = &context->response;
            response->setCode(200);
            response->set("Content-length", "17");
            // 此句是可以省略的
            // context->flush();
            context->write("Hello Sese Httpd!", 17);
            return;
        });
    });

    th.start();
    sese::sleep(5);
//    getchar();
    server->shutdown();
    th.join();
    return 0;
}