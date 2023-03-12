#pragma once

#include "sese/Config.h"

namespace sese {

    class API Process {
    public:
        using Ptr = std::unique_ptr<Process>;

        static Process::Ptr create(const char *command) noexcept;

        [[nodiscard]] int wait() const noexcept;

    private:
        Process() = default;

#ifdef _WIN32
    private:
        void *handle = nullptr;
#elif __linux__
    private:
        static void exec(const char *pCommand) noexcept;
        // 计算字符串中参数的个数
        static size_t count(const char *pCommand) noexcept;
        // 将下一个空格设置为 '\0'，并返回空格下一个字符的指针
        static char *spilt(char *pCommand) noexcept;

        pid_t id = -1;
#endif
    };
}// namespace sese