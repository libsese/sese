#pragma once

#include "sese/util/InputStream.h"

namespace sese {

    class API GZipFileInputStream : public InputStream {
    public:
        using Ptr = std::unique_ptr<GZipFileInputStream>;

        static GZipFileInputStream::Ptr create(const char *file) noexcept;

        int64_t read(void *buffer, size_t length) override;

        void close() noexcept;

    private:
        GZipFileInputStream() = default;

        void *gzFile = nullptr;
    };

}// namespace sese