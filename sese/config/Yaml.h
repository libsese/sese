/// \file Yaml.h
/// \brief Yaml 解析器
/// \author kaoru
/// \date 2023年11月4日

#pragma once

#include <sese/io/InputStream.h>
#include <sese/io/OutputStream.h>
#include <sese/util/Value.h>

#include <queue>

namespace sese {
/// Yaml 解析器
class Yaml {
    using Tokens = std::vector<std::string>;
    using TokensQueue = std::queue<std::tuple<int, Tokens>>;

    static Value parseObject(TokensQueue &tokens_queue, size_t level);

    static Value parseArray(TokensQueue &tokens_queue, size_t level);

    static void streamifyObject(io::OutputStream *output, const Value::Dict &dict, size_t level);

    static void streamifyArray(io::OutputStream *output, const Value::List &list, size_t level);

public:
    /// 从流中反序列化 yaml 对象
    /// \param input 输入流
    /// \param level 解析深度
    /// \return 解析失败则 Value::isNull 为真
    static Value parse(io::InputStream *input, size_t level);

    /// 向流中序列化 yaml 对象
    /// \param output 输出流
    /// \param value yaml 对象
    static void streamify(io::OutputStream *output, const Value &value);
};
} // namespace sese