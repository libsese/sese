/**
 * @file DateTimeFormatter.h
 * @date 2022年3月28日
 * @author kaoru
 * @brief 日期时间格式化类
 */
#pragma once
#include "sese/Config.h"
#include "sese/DateTime.h"
#include "sese/text/StringBuilder.h"

namespace sese::text {

    /**
     * @brief 日期时间格式化类
     * @verbatim
     时间格式通配符
     d    一个月中的某天(1-31)
     dd   一个月中的某天(01-31)
     ddd  一周某天的缩写(MonDays)
     dddd 一周某天的全称(Monday)
     m    分钟(0-59)
     mm   分钟(00-59)
     M    月份(1-12)
     MM   月份(01-12)
     MMM  月份缩写(Jun)
     MMMM 月份完整名称(June)
     y    年份(0-99)
     yy   年份(00-99)
     yyy  年份(000-999)
     yyyy 年份(0000-9999)
     z    相对于 UTC 偏移量
     h    12小时制(0-11)
     hh   12小时制(00-11)
     H    24小时制(0-23)
     HH   24小时制(00-23)
     t    AM / PM - 可配合 12 小时制使用
     s    秒数(0-59)
     ss   秒数(00-59)
     f    毫秒数(0-999)
     ff   毫秒数(000-999)
     fff  微秒数(0-999)
     ffff 微秒数(000-999)
     %    转义符号
     @endverbatim
     */
    class API DateTimeFormatter {
    public:
        static std::string format(const DateTime &dateTime, const std::string &pattern = TIME_DEFAULT_PATTERN);

        static std::string format(const DateTime::Ptr &dateTime, const std::string &pattern = TIME_DEFAULT_PATTERN);
    };
}// namespace sese