/**
 * @file AbstractAppender.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 日志输出源基类
 */
#pragma once
#include "AbstractFormatter.h"
#include "Config.h"

#ifdef _WIN32
#pragma warning (disable : 4251)
#endif

namespace sese {
    /**
     * @brief 日志输出源基类
     */
    class API AbstractAppender {
    public:
        typedef std::shared_ptr<AbstractAppender> Ptr;

        AbstractAppender() noexcept = default;

        AbstractAppender(AbstractFormatter::Ptr formatter, Level level) noexcept;

        /// 用于判断是否满足输出阈值
        void preDump(const Event::Ptr &event) noexcept;

        virtual void dump(const Event::Ptr &event) noexcept = 0;

    protected:
        /// 格式化器
        AbstractFormatter::Ptr formatter;
        /// 输出阈值
        Level level = Level::DEBUG;
    };
}// namespace sese