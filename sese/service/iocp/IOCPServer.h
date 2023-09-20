#pragma once

#include <sese/service/iocp/IOCPServer_V1.h>

namespace sese::iocp {
#if !defined(SESE_PLATFORM_WINDOWS)
    typedef Context_V1 Context;
    typedef IOCPServer_V1 IOCPServer;
#else
#endif
}// namespace sese::iocp