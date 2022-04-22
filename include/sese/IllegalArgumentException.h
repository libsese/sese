/**
 * @file IllegalArgumentException.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 非法参数异常类
 */
#pragma once
#include "sese/Config.h"
#include "sese/LogicException.h"

namespace sese {

    /**
     * @brief 非法参数异常类
     */
    class API IllegalArgumentException : public LogicException {
    public:
        explicit IllegalArgumentException(const std::string &str = EXCEPTION_ILLEGAL_ARGUMENT_STRING) : LogicException(str) {}
    };

}// namespace sese
