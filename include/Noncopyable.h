#pragma once
#include "Config.h"

namespace sese {

    class API Noncopyable {
    public:
        Noncopyable() = default;
        ~Noncopyable() = default;

        // 不可复制核心代码
        Noncopyable(const Noncopyable &) = delete;
        Noncopyable &operator=(const Noncopyable &) = delete;
    };
}// namespace sese