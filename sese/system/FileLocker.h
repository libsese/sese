/**
 * @file FileLocker.cpp
 * @brief kaoru
 * @author 文件锁
 */

#pragma once

#include <sese/Config.h>

namespace sese::system {

    /**
     * 文件锁
     */
    class API FileLocker {
    public:
        explicit FileLocker(int32_t fd) : fd(fd) {}

        bool lockWrite(int64_t start, int64_t len) const;
        bool lockRead(int64_t start, int64_t len) const;
        bool lock(int64_t start, int64_t len) const;
        bool unlock() const;

    private:
        int32_t fd{};
    };

}// namespace sese