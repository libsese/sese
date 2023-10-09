/**
 * @file ClassFactory.h
 * @brief 类工厂内建实现
 * @author kaoru
 * @version 0.2
 */

#pragma once

#include "sese/plugin/BaseClass.h"

#include <map>
#include <string>
#include <typeinfo>
#include <functional>
#include <initializer_list>

namespace sese::plugin {
/// 类工厂内建实现
class ClassFactory {
public:
    /// 注册信息
    struct RegisterInfo {
        const std::type_info *info;
        std::function<std::shared_ptr<BaseClass>()> builder;
    };
    using RegisterInfoMapType = std::map<std::string, RegisterInfo>;
    using ParamListType = std::initializer_list<std::pair<std::string, RegisterInfo>>;

    ClassFactory(const ParamListType &initializerList) noexcept;
    ClassFactory(ClassFactory &&classFactory) = delete;
    ClassFactory(const ClassFactory &classFactory) = delete;

    /// 创建某个已注册类的实例
    /// \param id 类注册名
    BaseClass::Ptr createClassWithId(const std::string &id) noexcept;

    /// 获取已注册类的类型信息
    /// \return 类信息映射表
    const RegisterInfoMapType &getRegisterClassInfo() noexcept;

protected:
    RegisterInfoMapType infoMap{};
};
} // namespace sese::plugin