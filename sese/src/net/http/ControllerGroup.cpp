#include <sese/net/http/ControllerGroup.h>
#include <sese/net/http/HttpUtil.h>

sese::net::http::ControllerGroup::ControllerGroup(const std::string &name) noexcept {
    this->name = name;
}

void sese::net::http::ControllerGroup::setController(const std::string &path, const sese::net::http::Controller &controller) noexcept {
    this->controllerMap[path] = controller;
}