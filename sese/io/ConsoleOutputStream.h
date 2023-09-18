/// \file ConsoleOutputStream.h
/// \brief 控制台输出流类 - 仅用于测试
/// \author kaoru
/// \version 0.1
/// \date 2022.11.20

#pragma once

#include "sese/io/OutputStream.h"

namespace sese::io {

    /// 控制台输出流类 - 仅用于测试
    class API ConsoleOutputStream final : public OutputStream {
    public:
        int64_t write(const void *buffer, size_t length) override;
    };
}