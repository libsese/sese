/**
 * @file AbstractFormatter.h
 * @brief 日志格式化类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <string>
#include "sese/record/Event.h"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::record {
/**
 * @brief 日志格式化类
 */
class API AbstractFormatter {
public:
    typedef std::shared_ptr<AbstractFormatter> Ptr;

    virtual ~AbstractFormatter() noexcept = default;

    virtual std::string dump(const Event::Ptr &event) noexcept = 0;
};
} // namespace sese::record