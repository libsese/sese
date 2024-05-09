/// \file StrCaseMap.h
/// \author kaoru
/// \date 2024年5月9日
/// \brief 忽略字符串键大小写的字典类

#pragma once

#include <map>
#include <unordered_map>
#include <string>

namespace sese {

/// \brief StrCaseMap 比较器
struct StrCaseMapComparer {
    bool operator()(const std::string &lv, const std::string &rv) const;
};

/// \brief StrCaseUnorderedMap 比较器
struct StrCaseUnorderedMapComparer {
    size_t operator()(const std::string &key) const;
    bool operator()(const std::string &lv, const std::string &rv) const;
};

/// \brief 忽略字符串键大小写的字典类
/// \tparam VALUE 字典值类型
/// \tparam ALLOCATOR 内存分配器
template<typename VALUE, typename ALLOCATOR = std::allocator<std::pair<const std::string, VALUE>>>
using StrCaseMap = std::map<std::string, VALUE, StrCaseMapComparer, ALLOCATOR>;

/// \brief 忽略字符串键大小写的字典类
/// \tparam VALUE 字典值类型
/// \tparam ALLOCATOR 内存分配器
template<typename VALUE, typename ALLOCATOR = std::allocator<std::pair<const std::string, VALUE>>>
using StrCaseUnorderedMap = std::unordered_map<std::string, VALUE, StrCaseUnorderedMapComparer, StrCaseUnorderedMapComparer, ALLOCATOR>;

} // namespace sese