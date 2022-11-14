/**
 * @file Stream.h
 * @date 2022年11月14日
 * @author kaoru
 * @brief 流式接口类
 * @version 0.2
 */
#pragma once
#include <sese/InputStream.h>
#include <sese/OutputStream.h>

namespace sese {

    /**
     * @brief 流式接口类
     */
    class API Stream : public InputStream, public OutputStream {
    public:
        typedef std::shared_ptr<Stream> Ptr;
        
        virtual void close() = 0;
    };

}// namespace sese