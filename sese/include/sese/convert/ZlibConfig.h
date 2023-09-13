/**
* @file ZlibConfig.h
* @author kaoru
* @version 0.1
* @brief ZLIB 相关配置声明
* @date 2023年9月13日
*/

#pragma once

#include "sese/Config.h"

namespace sese {
    enum class CompressionType {
        RAW = -30,
        ZLIB = 0,
        GZIP = 16
    };
}