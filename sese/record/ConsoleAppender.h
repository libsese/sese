/**
 * @file ConsoleAppender.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 控制台日志输出源类
 */
#pragma once

#include "sese/record/AbstractAppender.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"

namespace sese::record {
/**
 * @brief 控制台输出源类
 */
class API ConsoleAppender final : public AbstractAppender {
public:
    explicit ConsoleAppender(Level level = Level::DEBUG) noexcept;

    void dump(const char *buffer, size_t size) noexcept override;

public:
    static void setDebugColor() noexcept;
    static void setInfoColor() noexcept;
    static void setWarnColor() noexcept;
    static void setErrorColor() noexcept;
    static void setCleanColor() noexcept;
};
} // namespace sese::record