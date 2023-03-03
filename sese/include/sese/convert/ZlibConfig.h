#pragma once

#include "sese/Config.h"

namespace sese {
    enum class CompressionType {
        RAW = -30,
        ZLIB = 0,
        GZIP = 16
    };
}