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
    class API Base64Converter final : public NotInstantiable {
    public:
        static void encode(const InputStream::Ptr &src, const OutputStream::Ptr &dest);
        static void encode(InputStream *src, OutputStream *dest);

        static void decode(const InputStream::Ptr &src, const OutputStream::Ptr &dest);
        static void decode(InputStream *src, OutputStream *dest);

    public:
        using CodePage = const unsigned char *;

        static const unsigned char Base64CodePage[];
        static const unsigned char Base62CodePage[];

        static bool encode(const InputStream *input, OutputStream *output, CodePage page) noexcept;
        static bool decode(const InputStream *input, OutputStream *output, CodePage page) noexcept;

#ifdef SESE_BUILD_TEST
    public:
#else
    private:
#endif
        static void encodeInteger(size_t num, OutputStream *output, CodePage codePage) noexcept;
        static int64_t decodeBuffer(const unsigned char *buffer, size_t size, CodePage codePage) noexcept;
    };
}// namespace sese