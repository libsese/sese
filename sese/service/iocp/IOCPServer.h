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

/**
 * @file IOCPServer.h
 * @brief IOCP 服务
 * @author kaoru
 */

#pragma once

#include <sese/Config.h>

#if defined(SESE_PLATFORM_WINDOWS)
#include <sese/native/win/service/iocp/NativeIOCPServer_V1.h>
#else
#include <sese/service/iocp/IOCPServer_V1.h>
#endif

namespace sese::iocp {
#if defined(SESE_PLATFORM_WINDOWS)
typedef _windows::iocp::v1::NativeContext Context;
typedef _windows::iocp::v1::NativeIOCPServer IOCPServer;
namespace v1 {
    typedef _windows::iocp::v1::NativeContext Context;
    typedef _windows::iocp::v1::NativeIOCPServer IOCPServer;
}
#else
typedef v1::Context Context;
typedef v1::IOCPServer IOCPServer;
#endif
} // namespace sese::iocp