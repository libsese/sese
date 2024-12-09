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

#include <sese/io/FakeStream.h>

// The standard library stream is encapsulated and is not responsible for verification
// GCOVR_EXCL_START

sese::io::StdInputStreamWrapper::StdInputStreamWrapper(std::istream &stream) : stream(stream) {}

int64_t sese::io::StdInputStreamWrapper::read(void *buffer, size_t length) {
    stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length));
    return stream.gcount();
}

int64_t sese::io::StdInputStreamWrapper::peek(void *buffer, size_t length) {
    stream.read(static_cast<char *>(buffer), static_cast<std::streamsize>(length));
    const auto L = stream.gcount();
    stream.seekg(-L, std::ios_base::cur);
    return L;
}

int64_t sese::io::StdInputStreamWrapper::trunc(size_t length) {
    stream.seekg(static_cast<std::streamsize>(length), std::ios_base::cur);
    return stream.gcount();
}

sese::io::StdOutputStreamWrapper::StdOutputStreamWrapper(std::ostream &stream) : stream(stream), latest(stream.tellp()) {
}

int64_t sese::io::StdOutputStreamWrapper::write(const void *buffer, size_t length) {
    stream.write(static_cast<const char *>(buffer), static_cast<std::streamsize>(length));
    if (latest == -1) {
        // If the latest value is -1, the stream type does not support getting the write size,
        // and the write can only be assumed to be complete
        return static_cast<int64_t>(length);
    }
    auto now = stream.tellp();
    auto result = now - latest;
    latest = now;
    return result;
}

// GCOVR_EXCL_STOP