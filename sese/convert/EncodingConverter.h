/**
 * @file EncodingConverter.h
 * @author kaoru
 * @date 2022年4月15日
 * @brief 字符串编码转换
 */

#pragma once

#include "sese/Config.h"
#include "sese/util/NotInstantiable.h"
#include "sese/util/Initializer.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4624)
#endif

namespace sese {

    /**
     * @brief 字符串编码转换
     */
    class API SESE_DEPRECATED [[maybe_unused]] EncodingConverter : public NotInstantiable {
    public:
        [[maybe_unused]] static std::string toString(const std::wstring &wstring) noexcept;
        [[maybe_unused]] static std::wstring toWstring(const std::string &string) noexcept;
    };

    /// 编码转换器初始化任务
    class EncodingConverterInitiateTask final : public InitiateTask {
    public:
        EncodingConverterInitiateTask() : InitiateTask(__FUNCTION__) {}

        int32_t init() noexcept override;
        int32_t destroy() noexcept override;
    };
}// namespace sese