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

#include <sese/system/Path.h>

using namespace sese::system;

Path::Path(const std::string_view &unix_path) noexcept {
    Path::unix_path = unix_path;
    Path::valid = true;
}

Path::Path(const char *unix_path) noexcept : Path(std::string_view(unix_path)) {
}

Path Path::fromNativePath(const std::string &native_path) noexcept {
    Path result;
    result.unix_path = native_path;
    result.valid = true;
    return result;
}