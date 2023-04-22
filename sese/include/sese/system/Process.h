/// \file Process.h
/// \brief 进程类
/// \author kaoru
/// \version 0.1
/// \date 2023年3月13日

#pragma once

#include "sese/Config.h"

namespace sese::system {

    /// 进程类
    class API Process {
    public:
        using Ptr = std::unique_ptr<Process>;

        /// 创建一个新的进程对象
        /// \param command 命令
        /// \retval nullptr 创建失败
        static Process::Ptr create(char *command) noexcept;

        /// 获取当前进程 ID
        [[nodiscard]] static pid_t getCurrentProcessId() noexcept;

        /// 等待进程结束
        /// \return 进程退出码
        [[nodiscard]] int wait() const noexcept;

        /// 结束进程
        /// \return 操作结果
        [[nodiscard]] bool kill() const noexcept;

        /// 获取进程 ID
        [[nodiscard]] pid_t getProcessId() const noexcept;

    private:
        Process() = default;

#ifdef _WIN32
    public:
        virtual ~Process() noexcept;

    private:
        void *startupInfo = nullptr;
        void *processInfo = nullptr;
#else
    public:
        virtual ~Process() noexcept = default;

    private:
        // Unix-like 执行新进程实现
        static void exec(char *pCommand) noexcept;
        // 计算字符串中参数的个数
        static size_t count(const char *pCommand) noexcept;
        // 将下一个空格设置为 '\0'，并返回空格下一个字符的指针
        static char *spilt(char *pCommand) noexcept;

        pid_t id = -1;
#endif
    };
}// namespace sese