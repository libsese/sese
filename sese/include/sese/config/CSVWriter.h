/// \file CSVReader.h
/// \brief CSV 流写入器
/// \author kaoru
/// \version 0.1
/// \date 2023年3月30日

#pragma once

#include "sese/util/OutputStream.h"

#include <vector>
#include <string>

namespace sese {

    /// CSV 流写入器
    class API CSVWriter {
    public:
        using Row = std::vector<std::string>;

        static const char *CRLF;

        /// 构造函数
        /// \param dest 目的流
        /// \param split 分割字符
        explicit CSVWriter(OutputStream *dest, char splitChar = ',') noexcept;

        /// 写入流
        /// \param row 待写入行
        void write(const Row &row) noexcept;

    protected:
        char splitChar;
        OutputStream *dest = nullptr;
    };
}