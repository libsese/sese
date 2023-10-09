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
class API PercentConverter : public NotInstantiable {
public:
    using OutputStream = io::OutputStream;

    static void encode(const char *src, const OutputStream::Ptr &dest);

    static void encode(const char *src, const OutputStream::Ptr &dest, std::set<char> &excludeChars);

    static bool decode(const char *src, const OutputStream::Ptr &dest);

    static void encode(const char *src, OutputStream *dest);

    static void encode(const char *src, OutputStream *dest, std::set<char> &excludeChars);

    static bool decode(const char *src, OutputStream *dest);

    static std::string encode(const char *src);

    static std::string encode(const char *src, std::set<char> &excludeChars);

    /// 解码字符串
    /// \param src 带解码字符串
    /// \retval {} 解码失败
    static std::string decode(const char *src);

    static std::set<char> urlExcludeChars;
};
} // namespace sese