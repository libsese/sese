/**
 * @file SimpleFormatter.h
 * @brief 简单格式化类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include "sese/record/AbstractFormatter.h"
#include "sese/Config.h"
#include "sese/record/Event.h"

namespace sese::record {
/**
 * @brief 简单格式化类
 */
class  SimpleFormatter final : public AbstractFormatter {
public:
    /**
     * 默认构造函数
     * @param text_pattern 格式字符串，默认为 RECORD_DEFAULT_TEXT_PATTERN
     * @see RECORD_DEFAULT_TEXT_PATTERN
     * @verbatim
       %    转义符号
       li  行号
       lv  日志等级
       fn  文件名
       th  线程 ID
       tn  线程名称
       c   完整时间
       m   内容
     @endverbatim
     * @param time_pattern 时间格式字符串
     */
    explicit SimpleFormatter(const std::string &text_pattern = RECORD_DEFAULT_TEXT_PATTERN, const std::string &time_pattern = RECORD_DEFAULT_TIME_PATTERN) noexcept;

    std::string dump(const Event::Ptr &event) noexcept override;

    [[nodiscard]] const char *getTextPattern() const noexcept { return this->textPattern.c_str(); }

    [[nodiscard]] const char *getTimePattern() const noexcept { return this->timePattern.c_str(); }

private:
    std::string textPattern;
    std::string timePattern;
};
} // namespace sese::record