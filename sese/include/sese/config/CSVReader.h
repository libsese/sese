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
    class API CSVReader {
    public:
        using InputStream = io::InputStream;
        using Row = std::vector<std::string>;

        /// 构造函数
        /// \param source 读取的流
        /// \param splitChar 分割字符
        explicit CSVReader(InputStream *source, char splitChar = ',') noexcept;

        /// 从流中读取一行
        /// \return 返回包含一行内所有元素的 vector 容器
        Row read() noexcept;

    protected:
        char splitChar;
        InputStream *source = nullptr;
    };
}
