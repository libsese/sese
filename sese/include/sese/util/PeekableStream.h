#pragma once

#include "sese/Config.h"

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /// 可窥视流接口
    class API PeekableStream {
    public:
        typedef std::shared_ptr<PeekableStream> Ptr;

        virtual int64_t peek(void *buffer, size_t length) = 0;
    };
}