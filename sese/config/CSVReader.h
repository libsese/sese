/// \file CSVReader.h
/// \brief CSV 流读取器
/// \author kaoru
/// \version 0.1
/// \date 2023年3月30日

#pragma once

#include "sese/io/InputStream.h"

#include <vector>
#include <string>

namespace sese {

/// CSV 流读取器
class  CSVReader {
public:
    /// 工具支持的输入流
    using InputStream = io::InputStream;
    /// 行
    using Row = std::vector<std::string>;

    /// 构造函数
    /// \param source 读取的流
    /// \param split_char 分割字符
    explicit CSVReader(InputStream *source, char split_char = ',') noexcept;

    /// 从流中读取一行
    /// \return 返回包含一行内所有元素的 vector 容器
    Row read() noexcept;

protected:
    char splitChar;
    InputStream *source = nullptr;
};
} // namespace sese
