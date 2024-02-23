#include "sese/net/http/RequestableFactory.h"
#include "sese/internal/net/http/AsioHttpClient.h"

using namespace sese::net::http;

std::unique_ptr<Requestable> RequestableFactory::createHttpRequest(const std::string &url, const std::string &proxy) {
    auto rt = std::make_unique<AsioHttpClient>();
    if (rt->init(url, proxy)) {
        return rt;
    } else {
        return nullptr;
    }
}