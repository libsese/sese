/**
 * @file ConsoleAppender.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 控制台日志输出源类
 */
#pragma once
#include "sese/record/AbstractAppender.h"
#include <mutex>

namespace sese {
    /**
     * @brief 控制台输出源类
     */
    class API ConsoleAppender : public AbstractAppender {
    public:
        explicit ConsoleAppender(const AbstractFormatter::Ptr &formatter, Level level = Level::DEBUG) noexcept;

        void dump(const Event::Ptr &event) noexcept override;

    private:
        /// 互斥量
        std::mutex mutex;

    private:
        static void setDebugColor() noexcept;
        static void setInfoColor() noexcept;
        static void setWarnColor() noexcept;
        static void setErrorColor() noexcept;
        static void setCleanColor() noexcept;
    };
}// namespace sese