/**
* @file String.h
* @author kaoru
* @version 0.1
* @brief UTF-8 字符串
* @date 2023年9月13日
*/

#pragma once

#include <sese/text/SString.h>
#include <sese/text/SStringBuilder.h>

namespace sese::text {
    using Char = sstr::SChar;
    using String = sstr::SString;
    using StringView = sstr::SStringView;
}