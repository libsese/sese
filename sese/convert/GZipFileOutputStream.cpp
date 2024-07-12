#include <sese/convert/GZipFileOutputStream.h>
#include <zlib.h>

sese::GZipFileOutputStream::Ptr sese::GZipFileOutputStream::create(const char *file, size_t level) noexcept {
    auto gz_file = gzopen(file, "wb");
    if (!gz_file) return nullptr;
    auto l = std::min<int>(static_cast<int>(level), 9); // GCOVR_EXCL_LINE
    gzsetparams(gz_file, l, Z_DEFAULT_STRATEGY);
    auto p_stream = new GZipFileOutputStream;
    p_stream->gzFile = gz_file;
    return {p_stream, deleter};
}

int64_t sese::GZipFileOutputStream::write(const void *buffer, size_t length) {
    return gzwrite(static_cast<gzFile_s *>(gzFile), buffer, static_cast<unsigned int>(length));
}

void sese::GZipFileOutputStream::close() noexcept {
    gzclose(static_cast<gzFile_s *>(gzFile));
    gzFile = nullptr;
}

void sese::GZipFileOutputStream::deleter(GZipFileOutputStream *data) noexcept {
    delete data; // GCOVR_EXCL_LINE
}
