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

#include <sese/text/Util.h>

#include <cstdarg>

int sese::text::snprintf(char *buf, size_t buf_size, const char *pattern, ...) {
    int result;
    va_list args;
    va_start(args, pattern);
#if defined(SESE_PLATFORM_WINDOWS)
    result = vsprintf_s(buf, buf_size, pattern, args);
#elif defined(SESE_PLATFORM_APPLE)
    result = vsnprintf(buf, buf_size, pattern, args);
#else
    result = std::vsnprintf(buf, buf_size, pattern, args);
#endif
    va_end(args);
    return result;
}
