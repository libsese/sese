#include <sese/net/http/HttpConverter.h>

void sese::net::http::HttpConverter::convertFromHttp2(Request *request) {
    request->setVersion(HttpVersion::VERSION_2);
    request->setUrl(request->get(":path", "/"));
    auto method = request->get(":method", "get");
    if (strcmpDoNotCase(method.c_str(), "get")) {
        request->setType(RequestType::GET);
    } else if (strcmpDoNotCase(method.c_str(), "options")) {
        request->setType(RequestType::OPTIONS);
    } else if (strcmpDoNotCase(method.c_str(), "post")) {
        request->setType(RequestType::POST);
    } else if (strcmpDoNotCase(method.c_str(), "head")) {
        request->setType(RequestType::HEAD);
    } else if (strcmpDoNotCase(method.c_str(), "put")) {
        request->setType(RequestType::PUT);
    } else if (strcmpDoNotCase(method.c_str(), "delete")) {
        request->setType(RequestType::DELETE);
    } else if (strcmpDoNotCase(method.c_str(), "trace")) {
        request->setType(RequestType::TRACE);
    } else if (strcmpDoNotCase(method.c_str(), "connect")) {
        request->setType(RequestType::CONNECT);
    } else {
        request->setType(RequestType::ANOTHER);
    }
}

void sese::net::http::HttpConverter::convert2Http2(Response *response) {
    response->set(":status", std::to_string(response->getCode()));
}
