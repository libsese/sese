#pragma once
#include "Config.h"

namespace sese {

    class API NotInstantiable {
    public:
        NotInstantiable() = delete;
        ~NotInstantiable() = delete;
        NotInstantiable(const NotInstantiable &) = delete;
        NotInstantiable &operator=(const NotInstantiable &) = delete;
    };
}// namespace sese