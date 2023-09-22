#pragma once

#include <sese/Config.h>

#if defined(SESE_PLATFORM_WINDOWS)
#include <sese/native/Windows/service/iocp/IOCPServer_V1.h>
#else
#include <sese/service/iocp/IOCPServer_V1.h>
#endif

namespace sese::iocp {
#if defined(SESE_PLATFORM_WINDOWS)
    typedef _windows::iocp::Context_V1 Context;
    typedef _windows::iocp::IOCPServer_V1 IOCPServer;
#else
    typedef Context_V1 Context;
    typedef IOCPServer_V1 IOCPServer;
#endif
}// namespace sese::iocp