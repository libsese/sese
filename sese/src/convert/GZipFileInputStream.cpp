#include <sese/convert/GZipFileInputStream.h>
#include <zlib.h>

sese::GZipFileInputStream::Ptr sese::GZipFileInputStream::create(const char *file) noexcept {
    auto gzFile = gzopen(file, "rb");
    if (!gzFile) return nullptr;
    auto pStream = new GZipFileInputStream;
    pStream->gzFile = gzFile;
    return std::unique_ptr<GZipFileInputStream>(pStream);
}

int64_t sese::GZipFileInputStream::read(void *buffer, size_t length) {
    return gzread((gzFile_s *) gzFile, buffer, (unsigned int) length);
}

void sese::GZipFileInputStream::close() noexcept {
    gzclose((gzFile_s *) gzFile);
    gzFile = nullptr;
}
