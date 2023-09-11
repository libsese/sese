/**
 * \file InputStream.h
 * \date 2022年11月14日
 * \author kaoru
 * \brief 流式输入接口类
 * \version 0.1
 */
#pragma once

#include "sese/Config.h"

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::io {

    /// \brief 流式输入接口类
    class API InputStream {
    public:
        typedef std::shared_ptr<InputStream> Ptr;

        virtual int64_t read(void *buffer, size_t length) = 0;
    };

}// namespace sese