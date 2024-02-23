#pragma once

#include <sese/net/http/Requestable.h>

namespace sese::net::http {
    class RequestableFactory {
    public:
        static std::unique_ptr<Requestable> createHttpRequest(const std::string &url, const std::string &proxy = "");
    };
}