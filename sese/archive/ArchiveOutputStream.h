/// \file ArchiveOutputStream.h
/// \brief 存档输出流
/// \author kaoru
/// \date 2024年01月9日


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

#pragma once

#include <sese/io/OutputStream.h>

namespace sese::archive {

    ///存档输出流
    class ArchiveOutputStream : public io::OutputStream {
    public:
        explicit ArchiveOutputStream(void *archive_write);

        int64_t write(const void *buffer, size_t length) override;

    protected:
        void *archive_write{};
    };
}