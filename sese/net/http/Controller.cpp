#include <sese/net/http/Controller.h>
#include <sese/net/http/HttpUtil.h>
#include <cassert>

sese::net::http::Controller::Controller() {
    uri = "/";
    callback = [] (Request &req, Response &resp) {
        resp.setCode(404);
    };
}

sese::net::http::Controller::Controller(const std::string &url, Callback callback) : callback(std::move(callback)) {
    assert(!url.empty());
    assert(url.at(0) != '/');
    auto pos = url.find('?');
    if (pos == std::string::npos) {
        uri = url;
    } else {
        uri = url.substr(0, pos - 1);
    }
}

void sese::net::http::Controller::invoke(Request &req, Response &resp) const {
    callback(req, resp);
}

void sese::net::http::Controller::operator()(Request &req, Response &resp) const {
    invoke(req, resp);
}
