/**
 * @file Logger.h
 * @author kaoru
 * @date 2022年3月28日
 * @brief 日志输出类
 */
#pragma once
#include "sese/Config.h"
#include "sese/record/AbstractAppender.h"
#include "sese/record/AbstractFormatter.h"
#include "sese/record/Event.h"
#include "sese/Initializer.h"
#include <memory>
#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {

    class LoggerInitiateTask : public InitiateTask {
    public:
        LoggerInitiateTask() : InitiateTask(__FUNCTION__) {}

        int32_t init() noexcept override;
        int32_t destroy() noexcept override;
    };

    class ConsoleAppender;

    /**
      * @brief 日志输出类
      */
    class API Logger {
    public:
        /// 智能指针
        typedef std::shared_ptr<Logger> Ptr;

        /// 初始化
        Logger() noexcept;

        /**
         * 添加日志输出源
         * @param appender 日志输出源
         */
        void addAppender(const AbstractAppender::Ptr &appender) noexcept;

        /**
         * 输出日志
         * @param event 日志事件
         */
        void log(const Event::Ptr &event) const noexcept;

    private:
        std::shared_ptr<AbstractFormatter> formatter;
        std::shared_ptr<ConsoleAppender> builtInAppender;
        std::vector<AbstractAppender::Ptr> appenderVector;
    };

    /**
     * 获取全局 Logger 指针
     * @return Logger 指针
     */
    extern API record::Logger *getLogger() noexcept;
}// namespace sese::record
