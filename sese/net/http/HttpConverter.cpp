// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/net/http/HttpConverter.h>

bool sese::net::http::HttpConverter::convertFromHttp2(Request *request) {
    if (!request->exist(":path") ||
        !request->exist(":method") ||
        !request->exist(":scheme")
        ) {
        return false;
    }

    if (request->get(":path").empty()) {
        return false;
    }

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
        // request->setType(RequestType::ANOTHER);
        return false;
    }
    return true;
}

void sese::net::http::HttpConverter::convert2Http2(Response *response) {
    response->set(":status", std::to_string(response->getCode()));
}
