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

#include <sese/archive/ArchiveOutputStream.h>

#include <archive.h>

// GCOVR_EXCL_START

sese::archive::ArchiveOutputStream::ArchiveOutputStream(void *archive_write) :
        archive_write(archive_write) {
}

int64_t sese::archive::ArchiveOutputStream::write(const void *buffer, size_t length) {
    return archive_write_data(static_cast<struct  archive *>(this->archive_write), buffer, length);
}

// GCOVR_EXCL_STOP