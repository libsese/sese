#include <sese/service/http/HttpService_V3.h>
#include <sese/security/SSLContextBuilder.h>
#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>

using sese::net::http::Controller;
using sese::net::http::Request;
using sese::net::http::Response;

Controller my_ctl1("/info?name={name}&id={id}", [](Request &req, Response &resp) {
    auto name = req.getQueryArg("name", "");
    auto id = req.getQueryArg("id", "");

    if (name.empty() || id.empty()) {
        SESE_WARN("missing args");
        return;
    }

    std::string response = "Your name is '" + name + "', id is '" + id + "'.\n";
    resp.getBody().write(response.c_str(), response.size());
});

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto ssl = sese::security::SSLContextBuilder::SSL4Server();
    ssl->importCertFile(PROJECT_PATH "/test/Data/test-ca.crt");
    ssl->importPrivateKeyFile(PROJECT_PATH "/test/Data/test-key.pem");

    auto serv = sese::service::http::v3::HttpService::create();
    serv->setName("HttpServiceImpl_V3")
            .setAddress(sese::net::IPv4Address::localhost(9956))
            // .setSSLContext(ssl)
            .setKeepalive(60)
            .regController(my_ctl1);

    serv->startup();
    getchar();
    serv->shutdown();
}