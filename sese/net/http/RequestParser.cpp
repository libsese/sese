#include <sese/net/http/RequestParser.h>
#include <sese/net/AddressPool.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Util.h>

sese::net::http::RequestParser::Result sese::net::http::RequestParser::parse(const std::string &url) {
    Result result;
    result.url = Url(url);

    uint16_t port = 80;
    if (sese::StrCmpI()(result.url.getProtocol().c_str(), "Https") == 0) {
        port = 443;
    }

    IPv4Address::Ptr addr;
    if (result.url.getHost().find(':') == std::string::npos) {
        addr = parseAddress(result.url.getHost());
    } else {
        auto splitResult = text::StringBuilder::split(result.url.getHost(), ":");
        if (splitResult.size() == 2) {
            char *endPtr;
            port = (uint16_t) std::strtol(splitResult[1].c_str(), &endPtr, 10);
            if (*endPtr != 0) {
                return result;
            }
            addr = parseAddress(splitResult[0]);
        } else {
            return result;
        }
    }

    if (!addr) {
        return result;
    }

    addr->setPort(port);
    result.address = addr;

    result.request = std::make_unique<Request>();
    result.request->set("host", result.url.getHost());
    result.request->setUrl(result.url.getUrl() + result.url.getQuery());

    return result;
}

sese::net::IPv4Address::Ptr sese::net::http::RequestParser::parseAddress(const std::string &host) noexcept {
    auto result = text::StringBuilder::split(host, ".");
    if (result.size() == 4) {
        for (decltype(auto) item: result) {
            char *endPtr;
            auto bit = std::strtol(item.c_str(), &endPtr, 10);
            if (*endPtr != 0) {
                goto lookup;
            }
            if (bit < 0 || bit > 255) {
                goto lookup;
            }
        }
        return IPv4Address::create(host.c_str(), 0);
    }

lookup:
    return IPv4AddressPool::lookup(host);
}