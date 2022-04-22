/**
 * @file IndexOutOfBoundsException.h
 * @date 2022年3月28日
 * @author kaoru
 * @brief 越界异常类
 */
#pragma once
#include "sese/Config.h"
#include "sese/LogicException.h"

namespace sese {

    /**
     * @brief 越界异常类
     */
    class API IndexOutOfBoundsException : public LogicException {
    public:
        explicit IndexOutOfBoundsException(const std::string &str = EXCEPTION_INDEX_OUT_OF_BOUNDS_STRING) : LogicException(str) {}
    };

}// namespace sese
