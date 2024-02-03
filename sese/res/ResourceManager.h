/// \file ResourceManager.h
/// \brief 静态资源管理器
/// \author kaoru
/// \date 2024年02月4日


#pragma once

#include <sese/res/Resource.h>
#include <initializer_list>
#include <map>

namespace sese::res {

/// 静态资源管理器
class ResourceManager {
public:
    using MapType = std::map<std::string, Resource::Ptr>;

    ResourceManager() = default;

    virtual ~ResourceManager() = default;

    /// 根据资源名称获取资源
    /// \param name 资源名称
    /// \retval nullptr 资源不存在
    [[nodiscard]] Resource::Ptr getResource(const std::string &name) const;

    [[nodiscard]] decltype(auto) begin() const { return resourceMap.begin(); }

    [[nodiscard]] decltype(auto) end() const { return resourceMap.end(); }

protected:
    MapType resourceMap;
};

} // namespace sese::res