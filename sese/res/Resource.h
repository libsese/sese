#pragma once

#include <sese/res/ResourceStream.h>

namespace sese::res {

class API Resource {
public:
    using Ptr = std::shared_ptr<Resource>;

    Resource(std::string name, const void *buf, size_t size);

    ResourceStream::Ptr getStream();

    [[nodiscard]] const std::string &getName() const { return name; }

    [[nodiscard]] size_t getSize() const { return size; }

protected:
    std::string name{};
    const void *buf{};
    size_t size{};
};

} // namespace sese::res