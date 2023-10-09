/// \file PeekableStream.h
/// \author kaoru
/// \brief 可窥视流接口
/// \version 0.2
/// \date 2023年6月11日

#pragma once

#include "sese/Config.h"

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/// 可窥视流接口
class API PeekableStream {
public:
    typedef std::shared_ptr<PeekableStream> Ptr;

    /// 仅读取内容，不步进
    /// \param buffer 缓存
    /// \param length 缓存大小
    /// \return 实际读取大小
    virtual int64_t peek(void *buffer, size_t length) = 0;

    /// 仅步进，不读取内容
    /// \param length 步进大小
    /// \return 实际步进大小
    virtual int64_t trunc(size_t length) = 0;
};
} // namespace sese::io