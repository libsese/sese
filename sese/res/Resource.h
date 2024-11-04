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

/// \file Resource.h
/// \brief 静态资源
/// \author kaoru
/// \date 2024年02月4日

#pragma once

#include <sese/res/ResourceStream.h>

namespace sese::res {

/// 静态资源
class Resource { // GCOVR_EXCL_LINE
public:
    using Ptr = std::shared_ptr<Resource>;

    Resource(std::string name, const void *buf, size_t size);

    /// 获取静态资源流
    /// \return 流
    ResourceStream::Ptr getStream();

    /// 获取资源名称
    /// \return 资源名称
    [[nodiscard]] const std::string &getName() const { return name; }

    /// 获取资源大小
    /// \return 资源大小
    [[nodiscard]] size_t getSize() const { return size; }

protected:
    std::string name{};
    const void *buf{};
    size_t size{};
};

} // namespace sese::res