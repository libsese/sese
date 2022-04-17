#pragma once
#include "Stream.h"

namespace sese {

    class API Base64Converter {
    public:

        enum class CodePage {
            BASE64,
            BASE62
        };

        static void encode(const Stream::Ptr &src, const Stream::Ptr &dest, CodePage codePage = CodePage::BASE64);

        static void decode(const Stream::Ptr &src, const Stream::Ptr &dest);
    };
}