/// \file Resource.h
/// \brief 静态资源
/// \author kaoru
/// \date 2024年02月4日

#pragma once

#include <sese/res/ResourceStream.h>

namespace sese::res {

/// 静态资源
class API Resource {
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