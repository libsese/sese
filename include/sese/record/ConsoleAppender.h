/**
 * @file ConsoleAppender.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 控制台日志输出源类
 */
#pragma once
#include "AbstractAppender.h"

namespace sese {
    /**
     * @brief 控制台输出源类
     */
    class API ConsoleAppender : public AbstractAppender {
    public:
        explicit ConsoleAppender(const AbstractFormatter::Ptr &formatter, Level level = Level::DEBUG) noexcept;

        void dump(const Event::Ptr &event) noexcept override;

    private:
    };
}// namespace sese