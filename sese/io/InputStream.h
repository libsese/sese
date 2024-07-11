/**
 * \file InputStream.h
 * \date 2022年11月14日
 * \author kaoru
 * \brief 流式输入接口类
 * \version 0.1
 */
#pragma once

// GCOVR_EXCL_START

namespace sese::io {

/// \brief 流式输入接口类
class  InputStream {
public:
    typedef std::shared_ptr<InputStream> Ptr;

    virtual ~InputStream() noexcept = default;

    virtual int64_t read(void *buffer, size_t length) = 0;
};

} // namespace sese::io

// GCOVR_EXCL_STOP