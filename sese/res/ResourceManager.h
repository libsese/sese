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
/// \brief Static resource manager
/// \author kaoru
/// \date February 4, 2024

#pragma once

#include <sese/res/Resource.h>
#include <initializer_list>
#include <map>

namespace sese::res {

/// Static resource manager
class ResourceManager {
public:
    using MapType = std::map<std::string, Resource::Ptr>;

    ResourceManager() = default;

    virtual ~ResourceManager() = default; // GCOVR_EXCL_LINE

    /// \brief Get resource by name
    /// \param name Resource name
    /// \retval nullptr Resource does not exist
    [[nodiscard]] Resource::Ptr getResource(const std::string &name) const;

    [[nodiscard]] decltype(auto) begin() const { return resourceMap.begin(); }

    [[nodiscard]] decltype(auto) end() const { return resourceMap.end(); }

protected:
    MapType resourceMap;
};

} // namespace sese::res