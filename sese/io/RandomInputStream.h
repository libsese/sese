/// \file RandomInputStream.h
/// \brief 随机输入流
/// \author kaoru

#pragma once

#include <sese/io/InputStream.h>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese::io {

/// 随机输入流
class  RandomInputStream : public InputStream {
public:
    int64_t read(void *buffer, size_t length) override;
};

} // namespace sese::io