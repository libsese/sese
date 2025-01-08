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

#include <sese/util/MessageDigest.h>
#include <sese/util/MD5Util.h>
#include <sese/util/SHA1Util.h>
#include <sese/util/SHA256Util.h>

std::unique_ptr<char[]> sese::MessageDigest::digest(Type type, const InputStream::Ptr &source, bool is_cap) noexcept {
    switch (type) {
        case MD5:
            return sese::MD5Util::encode(source, is_cap);
        case SHA1:
            return sese::SHA1Util::encode(source, is_cap);
        default:
            return sese::SHA256Util::encode(source, is_cap);
    }
}

std::unique_ptr<char[]> sese::MessageDigest::digest(Type type, InputStream *source, bool is_cap) noexcept {
    switch (type) {
        case MD5:
            return sese::MD5Util::encode(source, is_cap);
        case SHA1:
            return sese::SHA1Util::encode(source, is_cap);
        default:
            return sese::SHA256Util::encode(source, is_cap);
    }
}