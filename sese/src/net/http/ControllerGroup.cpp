#include <sese/net/http/ControllerGroup.h>
#include <sese/net/http/HttpUtil.h>

sese::net::http::Controller sese::net::http::toController(const sese::net::http::SimpleController &controller) {
    return [controller](HttpConnection *conn) {
        auto rt = controller(conn->req, conn->resp);
        if (!rt) {
            conn->status = HttpHandleStatus::FAIL;
            return;
        }

        sese::net::http::HttpUtil::sendResponse(&conn->buffer2, &conn->resp);
        conn->status = HttpHandleStatus::OK;
    };
}

sese::net::http::ControllerGroup::ControllerGroup(const std::string &name) noexcept {
    this->name = name;
}

void sese::net::http::ControllerGroup::setController(const std::string &path, const sese::net::http::Controller &controller) noexcept {
    this->controllerMap[path] = controller;
}

void sese::net::http::ControllerGroup::setController(const std::string &path, const sese::net::http::SimpleController &controller) noexcept {
    this->controllerMap[path] = toController(controller);
}