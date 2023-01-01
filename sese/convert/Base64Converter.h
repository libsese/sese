/**
 * @file Base64Converter.h
 * @author kaoru
 * @date 2022年4月18日
 * @brief Base64 转换器
 */
#pragma once
#include "sese/util/Stream.h"
#include "sese/util/NotInstantiable.h"

#ifdef _WIN32
#pragma warning(disable : 4624)
#endif

namespace sese {

    /**
     * @brief Base64 转换器
     */
    class API Base64Converter : NotInstantiable {
    public:

        enum class CodePage {
            BASE64,
            BASE62
        };

        static void encode(const InputStream::Ptr &src, const OutputStream::Ptr &dest, CodePage codePage = CodePage::BASE64);

        static void decode(const InputStream::Ptr &src, const OutputStream::Ptr &dest);
    };
}