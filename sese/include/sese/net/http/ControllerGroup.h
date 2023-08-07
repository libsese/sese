#pragma once

#include <sese/net/http/HttpConnection.h>

#include <functional>

namespace sese::net::http {

    using Controller = std::function<void(Request &req, Response &resp)>;

    class API ControllerGroup {
    public:
        /// @brief 创建控制器组
        /// @param name 控制器组名
        /// @example /my_group
        /// @example /my_group/my_sub_group
        explicit ControllerGroup(const std::string &name) noexcept;

        [[nodiscard]] const std::string &getName() const { return name; }

        [[nodiscard]] const std::map<std::string, Controller> &getControllerMap() const { return controllerMap; }

        void setController(const std::string &path, const Controller &controller) noexcept;

    protected:
        std::string name;
        std::map<std::string, Controller> controllerMap;
    };

}// namespace sese::net::http