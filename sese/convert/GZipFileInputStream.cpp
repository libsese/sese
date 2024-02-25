#include <sese/convert/GZipFileInputStream.h>
#include <zlib.h>

sese::GZipFileInputStream::Ptr sese::GZipFileInputStream::create(const char *file) noexcept {
    auto gz_file = gzopen(file, "rb");
    if (!gz_file) return nullptr;
    auto p_stream = new GZipFileInputStream;
    p_stream->gzFile = gz_file;
    return {p_stream, deleter};
}

int64_t sese::GZipFileInputStream::read(void *buffer, size_t length) {
    return gzread((gzFile_s *) gzFile, buffer, (unsigned int) length);
}

void sese::GZipFileInputStream::close() noexcept {
    gzclose((gzFile_s *) gzFile);
    gzFile = nullptr;
}

void sese::GZipFileInputStream::deleter(sese::GZipFileInputStream *data) noexcept {
    delete data;
}
