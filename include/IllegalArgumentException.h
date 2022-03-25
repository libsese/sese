#pragma once
#include "Config.h"
#include "LogicException.h"

namespace sese {

    class API IllegalArgumentException : public LogicException {
    public:
        explicit IllegalArgumentException(const std::string &str = EXCEPTION_ILLEGAL_ARGUMENT_STRING) : LogicException(str) {}
    };

}// namespace sese
