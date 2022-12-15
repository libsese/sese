#pragma once
#include <sese/security/SSLContext.h>
#include <sese/NotInstantiable.h>

namespace sese::security {

    class API SSLContextBuilder : public NotInstantiable {
    public:
        static SSLContext::Ptr SSL4Client() noexcept;
        static SSLContext::Ptr SSL4Server() noexcept;
    };
}