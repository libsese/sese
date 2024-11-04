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

#include <sese/text/StringBuilder.h>

using sese::text::StringBuilder;

StringBuilder &operator<<(StringBuilder &stream, char ch) noexcept {
    stream.append(ch);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const char *str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const std::string &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const std::string_view &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const sese::text::String &str) noexcept {
    stream.append(str);
    return stream;
}

StringBuilder &operator<<(StringBuilder &stream, const sese::text::StringView &str) noexcept {
    stream.append(str);
    return stream;
}