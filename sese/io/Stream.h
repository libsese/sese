/**
 * @file Stream.h
 * @date 2022年11月14日
 * @author kaoru
 * @brief 流式接口类
 * @version 0.2
 */

#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

namespace sese::io {

/**
 * @brief 流式接口类
 */
class  Stream : public InputStream, public OutputStream {
public:
    typedef std::shared_ptr<Stream> Ptr;
};

} // namespace sese::io