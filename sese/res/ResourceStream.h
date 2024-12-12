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

/// \file ResourceStream.h
/// \brief Static resource stream
/// \author kaoru
/// \date February 4, 2024

#pragma once

#include <sese/io/PeekableStream.h>
#include <sese/io/InputStream.h>
#include <sese/io/File.h>

namespace sese::res {

/// Static resource stream
class ResourceStream : public io::PeekableStream, public io::InputStream {
public:
    using Ptr = std::unique_ptr<ResourceStream>;

    ResourceStream(const void *data, size_t size);

    int64_t read(void *buffer, size_t length) override;

    int64_t peek(void *buffer, size_t length) override;

    int64_t trunc(size_t length) override;

    [[nodiscard]] int64_t getSeek() const;

    int32_t setSeek(int64_t offset, io::Seek seek);

    int32_t setSeek(int64_t offset, int32_t whence);

    [[nodiscard]] size_t getSize() const { return size; }

protected:
    const void *data{};
    size_t size{};
    size_t pos{};
};

} // namespace sese::res