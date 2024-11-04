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
#include <sese/text/StringBuilder.h>

#include <cctype>

using namespace sese::system;

Path::Path(const std::string_view &unix_path) noexcept {
    Path::unix_path = unix_path;

    // 带盘符的绝对路径
    if (!Path::unix_path.empty() && Path::unix_path.at(0) == '/') {
        auto pos = Path::unix_path.find('/', 1);
        if (pos == std::string::npos) {
            valid = false;
            return;
        }
        if (pos - 1 != 1) {
            valid = false;
            return;
        }
        auto driver = Path::unix_path.substr(1, pos - 1);
        native_path = driver + ':' + (Path::unix_path.c_str() + 2);
        valid = true;
    }
    // 相对路径
    else {
        native_path = unix_path;
        valid = true;
    }
}

Path::Path(const char *unix_path) noexcept : Path(std::string_view(unix_path)) {
}

Path Path::fromNativePath(const std::string &native_path) noexcept {
    // 带盘符的绝对路径
    if (native_path.size() >= 3 && native_path.compare(1, 2, ":/") == 0 && isalpha(native_path.at(0))) {
        text::StringBuilder string_builder;
        string_builder.append('/');
        string_builder.append(native_path.at(0));
        string_builder.append(native_path.c_str() + 2, native_path.size() - 2);

        Path result;
        result.unix_path = string_builder.toString();
        result.native_path = native_path;
        result.valid = true;
        return result;
    }
    // 相对路径
    else if (!native_path.empty() && native_path.at(0) != '/') {
        Path result;
        result.unix_path = native_path;
        result.native_path = native_path;
        result.valid = true;
        return result;
    }
    // 非法路径
    else {
        return {};
    }
}

void Path::replaceWindowsPathSplitChar(char *path, size_t len) noexcept {
    for (size_t i = 0; i < len; i++) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }
}