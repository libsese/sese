/// \file Util.h
/// \brief 工具集合
/// \author kaoru
/// \version 0.1
#include <map>
#include <string>

namespace sese::db {

    /// \brief 对字符串进行简单的分词
    /// \example host=127.0.0.1;user=kaoru;pwd=xxx;db=tb_user; -> {host, 127.0.0.1}, ...
    /// \param string 目标字符串
    /// \return 分词结果
    std::map<std::string, std::string> tokenize(const char *string) noexcept;
}