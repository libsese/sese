/**
 * @file FileException.h
 * @brief 文件异常类
 * @date 2022年4月11日
 * @author kaoru
 */
#pragma once
#include "IOException.h"

namespace sese {

    /**
     * @brief 文件异常类
     */
    class API FileException : public IOException {
    public:
        explicit FileException(const std::string &str = EXCEPTION_FILE) : IOException(str) {}
    };

}// namespace sese