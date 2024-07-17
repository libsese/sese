/**
 * @file AbstractAppender.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 日志输出源基类
 */

#pragma once

#include "sese/Config.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {

/**
 * @brief 日志输出源基类
 */
class  AbstractAppender {
public:
    typedef std::shared_ptr<AbstractAppender> Ptr;

    AbstractAppender() noexcept = default;

    virtual ~AbstractAppender() noexcept = default;

    explicit AbstractAppender(Level level) noexcept;

    virtual void dump(const char *buffer, size_t size) noexcept = 0;

    [[nodiscard]] Level getLevel() const noexcept { return level; }

    void setLevel(Level lv) noexcept { this->level = lv; }

protected:
    /// 输出阈值
    Level level = Level::DEBUG;
};

} // namespace sese::record