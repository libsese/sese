#pragma once

#include <sese/util/InputStream.h>
#include <sese/util/OutputStream.h>
#include <sese/util/PeekableStream.h>

namespace sese {

    class API FixedBuffer
        : public InputStream,
          public OutputStream,
          public PeekableStream {
    public:
        explicit FixedBuffer(size_t size) noexcept;
        virtual ~FixedBuffer() noexcept;
        // copy
        FixedBuffer(const FixedBuffer &buffer) noexcept;
        // move
        FixedBuffer(FixedBuffer &&buffer) noexcept;

        int64_t read(void *buffer, size_t length) override;
        int64_t write(const void *buffer, size_t length) override;
        int64_t peek(void *buffer, size_t length) override;
        void reset() noexcept;

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