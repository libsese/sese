#pragma once

#include <sese/net/http/HttpServletContext.h>
#include <sese/net/http/Range.h>
#include <sese/io/File.h>
#include <sese/util/StopWatch.h>

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
    sese::net::IPAddress::Ptr remote_address{};
    bool keepalive = false;
    sese::StopWatch stopwatch;
};

}