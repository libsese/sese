// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#include "sese/util/Initializer.h"

#include <memory>
#include <vector>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {

/// 日志器初始化任务
class LoggerInitiateTask final : public InitiateTask {
public:
    LoggerInitiateTask() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override;
    int32_t destroy() noexcept override;
};

class ConsoleAppender;

/**
 * @brief 日志输出类
 */
class  Logger {
public:
    /// 智能指针
    typedef std::shared_ptr<Logger> Ptr;

    /// 初始化
    Logger() noexcept;

    virtual ~Logger() noexcept = default;

    /**
     * 添加日志输出源
     * @param appender 日志输出源
     */
    void addAppender(const AbstractAppender::Ptr &appender) noexcept;

    /**
     * 移除日志输出源
     * @param appender 日志输出源
     */
    void removeAppender(const AbstractAppender::Ptr &appender) noexcept;

    /**
     * 输出日志
     * @param event 日志事件
     */
    virtual void log(const Event::Ptr &event) noexcept;

    /**
     * 输出原始内容
     * @param buffer
     * @param length
     */
    virtual void dump(const void *buffer, size_t length) noexcept;

protected:
    std::shared_ptr<AbstractFormatter> formatter;
    std::shared_ptr<ConsoleAppender> builtInAppender;
    std::vector<AbstractAppender::Ptr> appenderVector;

public:
    /// 为全局日志器添加日志输出源
    /// \param appender 日志输出源
    static void addGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept;

    /// 为全局日志器移除日志输出源
    /// \param appender 日志输出源
    static void removeGlobalLoggerAppender(const AbstractAppender::Ptr &appender) noexcept;
};

/**
 * 获取全局 Logger 指针
 * @return Logger 指针
 */
extern  record::Logger *getLogger() noexcept;
} // namespace sese::record
