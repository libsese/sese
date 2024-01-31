#pragma once

#include <sese/io/PeekableStream.h>
#include <sese/io/InputStream.h>

namespace sese::res {

class API ResourceStream : public io::PeekableStream, public io::InputStream {
public:
    using Ptr = std::unique_ptr<ResourceStream>;

    ResourceStream(const void *buf, size_t size);

    int64_t read(void *buffer, size_t length) override;

    int64_t peek(void *buffer, size_t length) override;

    int64_t trunc(size_t length) override;

    [[nodiscard]] int64_t getSeek() const;

    int32_t setSeek(int64_t offset, int32_t whence);

    [[nodiscard]] size_t getSize() const { return size; }

protected:
    const void *buf{};
    size_t size{};
    size_t pos{};
};

} // namespace sese::res