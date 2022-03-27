#pragma once
#include "Config.h"
#include <memory>

namespace sese {

    class API Stream {
    public:
        typedef std::shared_ptr<Stream> Ptr;

    public:
        virtual int64_t read(void *buffer, size_t length) = 0;
        virtual int64_t write(void *buffer, size_t length) = 0;
        virtual void close() = 0;
    };

}// namespace sese