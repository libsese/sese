/**
 * @file IOCPServer.h
 * @brief IOCP 服务
 * @author kaoru
 */

#pragma once

#include <sese/Config.h>

#if defined(SESE_PLATFORM_WINDOWS)
#include <sese/native/Windows/service/iocp/NativeIOCPServer_V1.h>
#else
#include <sese/service/iocp/IOCPServer_V1.h>
#endif

namespace sese::iocp {
#if defined(SESE_PLATFORM_WINDOWS)
typedef _windows::iocp::v1::NativeContext Context;
typedef _windows::iocp::v1::NativeIOCPServer IOCPServer;
#else
typedef v1::Context Context;
typedef v1::IOCPServer IOCPServer;
#endif
} // namespace sese::iocp