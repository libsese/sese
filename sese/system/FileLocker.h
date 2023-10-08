#pragma once

#include <sese/Config.h>

namespace sese::system {

    class API FileLocker {
    public:
        explicit FileLocker(int32_t fd) : fd(fd) {}

        bool lockWrite(int64_t start, int64_t len);
        bool lockRead(int64_t start, int64_t len);
        bool lockReadWrite(int64_t start, int64_t len);
        bool lock(int64_t start, int64_t len);
        bool unlock();

    private:
        int32_t fd{};
    };

}// namespace sese