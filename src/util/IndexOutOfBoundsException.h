#pragma once
#include "Exception.h"
#include "Config.h"

namespace sese {

    class IndexOutOfBoundsException : public Exception {
    public:
        explicit IndexOutOfBoundsException(const std::string &str = INDEX_OUT_OF_BOUNDS_STRING) : Exception(str) {}
    };

}// namespace sese
