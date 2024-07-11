/// \file DateTimeParser.h
/// \brief 日期解析器
/// \author kaoru
/// \date 2023年12月29日


#pragma once

#include <sese/Config.h>
#include <sese/util/DateTime.h>

#include <optional>

namespace sese::text {

/// 日期解析器
class  DateTimeParser {
public:
    /**
     * 尝试将字符串还原为数据结构
     * \param format 时间匹配格式
     * \verbatim
     * yyyy 完整年份
     * MM   完整月份
     * dd   完整某一个月的一天
     * HH   24小时制小时
     * mm   完整分钟数
     * ss   完整秒数
     * fff  完整毫秒数
     * rrr  完整微秒数
     * %    转义符号
     * '*'  单词
     * z    UTC 偏移量
     * \endverbatim
     * \param datetime 时间字符串
     * \return 时间数据结构
     */
    static std::optional<DateTime> parse(const std::string &format, const std::string &datetime);

private:
    static int count(const char *string, int max);
};

} // namespace sese::text