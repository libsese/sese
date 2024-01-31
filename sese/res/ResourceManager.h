#pragma once

#include <sese/res/Resource.h>
#include <initializer_list>
#include <map>

namespace sese::res {

class ResourceManager {
public:
    using MapType = std::map<std::string, Resource::Ptr>;

    ResourceManager() = default;

    virtual ~ResourceManager() = default;

    [[nodiscard]] Resource::Ptr getResouce(const std::string &name) const;

protected:
    MapType resourceMap;
};

} // namespace sese::res