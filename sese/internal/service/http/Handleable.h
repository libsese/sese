#pragma once

#include <sese/net/http/Request.h>
#include <sese/net/http/Response.h>
#include <sese/net/http/Range.h>
#include <sese/io/File.h>

#include <sese/internal/service/http/ConnType.h>

namespace sese::internal::service::http {

struct Handleable {
    using Ptr = std::shared_ptr<Handleable>;

    ConnType conn_type = ConnType::NONE;
    sese::net::http::Request request;
    sese::net::http::Response response;
    std::string content_type = "application/x-";
    io::File::Ptr file;
    size_t filesize = 0;
    std::vector<sese::net::http::Range> ranges;
    std::vector<sese::net::http::Range>::iterator range_iterator = ranges.begin();
    bool keepalive = false;
};

}