#include "../HttpServiceImpl.h"

#include <sese/Init.h>
#include <sese/security/SSLContextBuilder.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);

    auto any = sese::net::IPv4Address::any(80);
    auto ssl = sese::security::SSLContextBuilder::UniqueSSL4Server();
    ssl->importCertFile(PROJECT_PATH "/sese/sese/test/Data/test-ca.crt");
    ssl->importPrivateKeyFile(PROJECT_PATH "/sese/sese/test/Data/test-key.pem");
    HttpServiceImpl::MountPointMap mountPoints;
    HttpServiceImpl::ServletMap servlets;
    HttpServiceImpl::FilterMap filters;
    std::string name = "Hello";
    auto impl = std::make_shared<HttpServiceImpl>(any, nullptr, 30, name, mountPoints, servlets, filters);
    // auto impl = std::make_shared<HttpServiceImpl>(any, std::move(ssl), 30, name, mountPoints, servlets, filters);
    impl->startup();
    getchar();
    impl->shutdown();
}