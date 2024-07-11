/// \file NullOutputStream.h
/// \author kaoru
/// \brief 黑洞输出流

#pragma once

#include <sese/Config.h>
#include <sese/io/OutputStream.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/// 黑洞输出流
class  NullOutputStream : public OutputStream {
public:
    int64_t write(const void *buffer, size_t length) override;
};

} // namespace sese::io