/**
 * @file LogicException.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 逻辑异常类
 */
#pragma once
#include "Exception.h"

namespace sese {

    /**
     * @brief 逻辑异常类
     */
    class API LogicException : public Exception {
    public:
        explicit LogicException(const std::string &str = EXCEPTION_LOGIC_STRING) : Exception(str) {}
    };

}// namespace sese
