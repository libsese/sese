/**
 * @file Stream.h
 * @date 2022年3月28日
 * @author kaoru
 * @brief 流式接口类
 */
#pragma once
#include "Config.h"

namespace sese {

    /**
     * @brief 流式接口类
     */
    class API Stream {
    public:
        typedef std::shared_ptr<Stream> Ptr;

    public:
        virtual int64_t read(void *buffer, size_t length) = 0;
        virtual int64_t write(void *buffer, size_t length) = 0;
        virtual void close() = 0;
    };

}// namespace sese