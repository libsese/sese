/// \file ConsoleOutputStream.h
/// \brief 控制台输出流类 - 仅用于测试
/// \author kaoru
/// \version 0.1
/// \date 2022.11.20
#pragma once
#include "OutputStream.h"

namespace sese {

    /// 控制台输出流类 - 仅用于测试
    class API ConsoleOutputStream : public OutputStream {
    public:
        int64_t write(const void *buffer, size_t length) override;
    };
}