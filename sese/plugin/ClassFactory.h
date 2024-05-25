/**
 * @file ClassFactory.h
 * @brief 类工厂内建实现
 * @author kaoru
 * @version 1.0
 */

#pragma once

#include "sese/plugin/BaseClass.h"

#include <map>
#include <string>
#include <typeinfo>
#include <functional>

namespace sese::plugin {
/// 类工厂内建接口
class ClassFactory {
public:
    /// 注册信息
    struct RegisterInfo {
        const std::type_info *info;
        std::function<std::shared_ptr<BaseClass>()> builder;
    };
    using RegisterInfoMapType = std::map<std::string, RegisterInfo>;

    ClassFactory() noexcept = default;
    ClassFactory(ClassFactory &&class_factory) = delete;
    ClassFactory(const ClassFactory &class_factory) = delete;
    virtual ~ClassFactory() = default;

    virtual void init() = 0;

    /// 创建某个已注册类的实例
    /// \param id 类注册名
    BaseClass::Ptr createClassWithName(const std::string &id) noexcept;

    /// 获取已注册类的类型信息
    /// \return 类信息映射表
    const RegisterInfoMapType &getRegisterClassInfo() noexcept;

protected:
    RegisterInfoMapType infoMap{};
};
} // namespace sese::plugin