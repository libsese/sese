/// \file CSVWriter.h
/// \brief CSV 流写入器
/// \author kaoru
/// \version 0.1
/// \date 2023年3月30日

#pragma once

#include "sese/io/OutputStream.h"

#include <vector>
#include <string>

namespace sese {

/// CSV 流写入器
class API CSVWriter {
public:
    /// 工具支持的输出流
    using OutputStream = io::OutputStream;
    /// 行
    using Row = std::vector<std::string>;

    static const char *crlf;
    static const char *lf;

    /// 构造函数
    /// \param dest 目的流
    /// \param split_char 分割字符
    /// \param crlf 是否使用 CRLF 作为行分隔符
    explicit CSVWriter(OutputStream *dest, char split_char = ',', bool crlf = true) noexcept;

    /// 写入流
    /// \param row 待写入行
    void write(const Row &row) noexcept;

protected:
    char splitChar;
    bool isCRLF;
    OutputStream *dest = nullptr;
};
} // namespace sese