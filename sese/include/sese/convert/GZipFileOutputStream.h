#pragma once

#include "sese/util/OutputStream.h"

namespace sese {

    class API GZipFileOutputStream : public OutputStream {
    public:
        using Ptr = std::unique_ptr<GZipFileOutputStream>;

        static GZipFileOutputStream::Ptr create(const char *file, size_t level) noexcept;

        int64_t write(const void *buffer, size_t length) override;

        void close() noexcept;

    private:
        GZipFileOutputStream() = default;

        void *gzFile = nullptr;
    };

}// namespace sese