/// \file SpinLock.h
/// \brief 自旋锁
/// \author kaoru

#pragma once

#include <sese/Config.h>

#include <atomic>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4251)
#endif

namespace sese {
/// 自旋锁
class  SpinLock {
public:
    void lock();

    void unlock();

private:
    std::atomic_bool flag{false};
};
} // namespace sese