#include <sese/convert/GZipFileOutputStream.h>
#include <zlib.h>

sese::GZipFileOutputStream::Ptr sese::GZipFileOutputStream::create(const char *file, size_t level) noexcept {
    auto gzFile = gzopen(file, "wb");
    if (!gzFile) return nullptr;
    auto l = std::min<int>((int) level, 9);
    gzsetparams(gzFile, l, Z_DEFAULT_STRATEGY);
    auto pStream = new GZipFileOutputStream;
    pStream->gzFile = gzFile;
    return std::unique_ptr<GZipFileOutputStream>(pStream);
}

int64_t sese::GZipFileOutputStream::write(const void *buffer, size_t length) {
    return gzwrite((gzFile_s *) gzFile, buffer, (unsigned int) length);
}

void sese::GZipFileOutputStream::close() noexcept {
    gzclose((gzFile_s *) gzFile);
    gzFile = nullptr;
}
