/// \file Module.h
/// \author kaoru
/// \date 2023年3月21日
/// \version 0.1.0
/// \brief 插件模块对象，适配 sese-plugin 相关接口

#pragma once

#include "sese/plugin/Marco.h"
#include "sese/system/LibraryLoader.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::plugin {

/// 插件模块对象
class API Module {
public:
    using Ptr = std::unique_ptr<Module>;
    using LibraryObject = sese::system::LibraryObject;

    /// 尝试从动态链接库中加载一个模块
    /// \param path 动态链接库路径
    /// \retval nullptr 加载错误
    static Module::Ptr open(const std::string &path) noexcept;

    /// 尝试从动态链接库中加载一个模块
    /// \param path 动态链接库路径
    /// \retval nullptr 加载错误
    static Module::Ptr openWithPath(const system::Path &path) noexcept;

    /// 获取模块名称
    /// \return 模块名称
    const char *getName() noexcept;

    /// 获取模块版本
    /// \return 模块版本
    const char *getVersion() noexcept;

    /// 获取模块描述
    /// \return 模块描述
    const char *getDescription() noexcept;

    /// 使用模块的类工厂构建一个对象
    /// \param id 对象标识符
    /// \retval nullptr 实例化错误
    BaseClass::Ptr createClassWithId(const std::string &id) noexcept;

    /// 使用模块的类工厂构建一个对象并转换为指定类型
    /// \tparam type 指定类型
    /// \param id 对象标识符
    /// \retval nullptr 实例化错误
    template<typename type>
    std::shared_ptr<type> createClassWithIdAs(const std::string &id) {
        auto p = createClassWithId(id);
        if (p) {
            return std::reinterpret_pointer_cast<type>(p);
        } else {
            return nullptr;
        }
    }

    /// 获取模块已注册类类型信息
    /// \return 类型信息
    const ClassFactory::RegisterInfoMapType &getRegisterClassInfo() noexcept;

private:
    Module() = default;

    LibraryObject::Ptr object = nullptr;

    ModuleInfo *info = nullptr;
    ClassFactory *factory = nullptr;
};
} // namespace sese::plugin