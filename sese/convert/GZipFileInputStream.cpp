// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
    return gzread(static_cast<gzFile_s *>(gzFile), buffer, static_cast<unsigned int>(length));
}

void sese::GZipFileInputStream::close() noexcept {
    gzclose(static_cast<gzFile_s *>(gzFile));
    gzFile = nullptr;
}

void sese::GZipFileInputStream::deleter(GZipFileInputStream *data) noexcept {
    delete data; // GCOVR_EXCL_LINE
}
