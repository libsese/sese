/**
 * @file ControllerGroup.h
 * @brief 控制器组
 * @author kaoru
 * @version 0.1
 * @date 2023年9月13日
 */

#pragma once

#include <sese/net/http/HttpConnection.h>

#include <functional>

namespace sese::net::http {

/// HTTP 控制器
using Controller = std::function<void(Request &req, Response &resp)>;

/// 控制器组
class API ControllerGroup {
public:
    /** @brief 创建控制器组
     *  @param name 控制器组名
     *  @verbatim
     *  /my_group
     *  /my_group/my_sub_group
     *  @endverbatim
     */
    explicit ControllerGroup(const std::string &name) noexcept;

    /// 获取控制器组名称
    /// \return 控制器组名称
    [[nodiscard]] const std::string &getName() const { return name; }

    /// 获取控制器组中的控制器映射
    /// \return 控制器映射
    [[nodiscard]] const std::map<std::string, Controller> &getControllerMap() const { return controllerMap; }

    /// 为控制器组添加控制器
    /// \param path 子路径
    /// \param controller 控制器
    void setController(const std::string &path, const Controller &controller) noexcept;

protected:
    std::string name;
    std::map<std::string, Controller> controllerMap;
};

} // namespace sese::net::http