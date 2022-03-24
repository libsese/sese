#pragma once
#include "Config.h"
#include "LogicException.h"

namespace sese {

    class API IndexOutOfBoundsException : public LogicException {
    public:
        explicit IndexOutOfBoundsException(const std::string &str = EXCEPTION_INDEX_OUT_OF_BOUNDS_STRING) : LogicException(str) {}
    };

}// namespace sese
