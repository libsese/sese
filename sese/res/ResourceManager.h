// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

    virtual ~ResourceManager() = default; // GCOVR_EXCL_LINE

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