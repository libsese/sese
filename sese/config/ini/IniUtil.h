/**
* \file IniUtil.h
* \author kaoru
* \brief INI 工具类
* \date 2023年9月13日
*/

#pragma once

#include <sese/config/ini/IniConfig.h>
#include <sese/io/PeekableStream.h>
#include <sese/io/OutputStream.h>

namespace sese::ini {

/// INI 工具类
class  IniUtil {
public:
    /// 工具支持的输入流
    using InputStream = io::PeekableStream;
    /// 工具支持的输出流
    using OutputStream = io::OutputStream;
    /// INI CONFIG 对象
    using IniConfigObject = std::unique_ptr<IniConfig>;

    /**
     * 从可窥视流中解析一个 INI CONFIG 对象
     * @param input 欲处理的可窥视流
     * @return INI CONFIG 对象
     */
    static IniConfigObject parse(InputStream *input) noexcept;

    /**
     * 向流中写入 INI CONFIG 对象
     * @param target 欲写入的 INI CONFIG
     * @param output 欲输出的流
     * @return 操作是否成功
     */
    static bool unparse(const IniConfigObject &target, OutputStream *output) noexcept;

protected:
    static bool unparsePair(const std::pair<std::string, std::string> &pair, OutputStream *output) noexcept;
};

} // namespace sese::ini