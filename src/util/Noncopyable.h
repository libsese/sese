#pragma once

namespace sese {

    class Noncopyable {
    public:
        Noncopyable() = default;
        ~Noncopyable() = default;

        // 不可复制核心代码
        Noncopyable(const Noncopyable &) = delete;
        Noncopyable &operator=(const Noncopyable &) = delete;
    };
}// namespace sese