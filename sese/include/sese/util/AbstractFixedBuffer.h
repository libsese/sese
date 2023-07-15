#pragma once

#include <sese/util/Stream.h>
#include <sese/util/PeekableStream.h>

namespace sese {

    class API AbstractFixedBuffer : public Stream, public PeekableStream {
    public:
        explicit AbstractFixedBuffer(size_t size) noexcept;
        virtual ~AbstractFixedBuffer() noexcept;
        // copy
        AbstractFixedBuffer(const AbstractFixedBuffer &buffer) noexcept;
        // move
        AbstractFixedBuffer(AbstractFixedBuffer &&buffer) noexcept;

        int64_t read(void *buffer, size_t length) override;
        int64_t write(const void *buffer, size_t length) override;
        int64_t peek(void *buffer, size_t length) override;
        int64_t trunc(size_t length) override;
        virtual void reset() noexcept;

        [[nodiscard]] const char *data() const { return buffer; }
        [[nodiscard]] size_t getSize() const noexcept { return size; }
        [[nodiscard]] size_t getReadSize() const noexcept { return readSize; }
        [[nodiscard]] size_t getWriteSize() const noexcept { return writeSize; }
        [[nodiscard]] size_t getReadableSize() const noexcept { return writeSize - readSize; }
        [[nodiscard]] size_t getWriteableSize() const noexcept { return size - writeSize; }

    protected:
        size_t size{};
        size_t readSize{};
        size_t writeSize{};
        char *buffer{};
    };

}// namespace sese