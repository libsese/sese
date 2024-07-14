/**
 * \file OutputStream.h
 * \date 2022年11月14日
 * \author kaoru
 * \brief 流式输出接口类
 * \version 0.1
 */
#pragma once

#include "sese/Config.h"

namespace sese::io {

// GCOVR_EXCL_START

/// \brief 流式输出接口类
class  OutputStream {
public:
    typedef std::shared_ptr<OutputStream> Ptr;

    virtual ~OutputStream() noexcept = default;

    virtual int64_t write(const void *buffer, size_t length) = 0;
};

// GCOVR_EXCL_STOP

} // namespace sese::io