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

/// \file NullOutputStream.h
/// \author kaoru
/// \brief 黑洞输出流

#pragma once

#include <sese/Config.h>
#include <sese/io/OutputStream.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/// 黑洞输出流
class  NullOutputStream : public OutputStream {
public:
    int64_t write(const void *buffer, size_t length) override;
};

} // namespace sese::io