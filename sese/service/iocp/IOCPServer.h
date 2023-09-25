#pragma once

#include <sese/Config.h>

#if defined(SESE_PLATFORM_WINDOWS)
#include <sese/native/Windows/service/iocp/NativeIOCPServer_V1.h>
#else
#include <sese/service/iocp/IOCPServer_V1.h>
#endif

namespace sese::iocp {
#if defined(SESE_PLATFORM_WINDOWS)
    typedef _windows::iocp::NativeContext_V1 Context_V1;
    typedef _windows::iocp::NativeIOCPServer_V1 IOCPServer_V1;
#endif
    typedef Context_V1 Context;
    typedef IOCPServer_V1 IOCPServer;
}// namespace sese::iocp