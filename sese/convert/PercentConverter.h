/**
 * @file PercentConverter.h
 * @author kaoru
 * @date 2022年4月18日
 * @brief 百分号编码转换器
 */
#pragma once
#include "sese/io/OutputStream.h"
#include "sese/util/NotInstantiable.h"

#include <set>

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese {

/**
 * @brief 百分号编码转换器
 */
class  PercentConverter final : public NotInstantiable {
public:
    using OutputStream = io::OutputStream;

    PercentConverter() = delete;

    static void encode(const char *src, const OutputStream::Ptr &dest);

    static bool decode(const char *src, const OutputStream::Ptr &dest);

    static void encode(const char *src, OutputStream *dest);

    static bool decode(const char *src, OutputStream *dest);

    static std::string encode(const char *src);

    /// 解码字符串
    /// \param src 带解码字符串
    /// \retval {} 解码失败
    static std::string decode(const char *src);

private:
    static const std::set<char> URL_EXCLUDE_CHARS;
};
} // namespace sese