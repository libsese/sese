/**
 * @file Config.h
 * @brief Windows 平台配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include <Windows.h>

#ifdef WINDOWS_DLL
/// 可导出类型标识符
#define API __declspec(dllexport)
#else
/// 可导出类型标识符
#define API
#endif

#ifdef NEED_DBGHELP
#pragma comment(lib, "DbgHelp.lib")
#endif

using pid_t = uint32_t;
/// pid_t 格式化占位符
#define PRIdPid "u"
/// 忽略大小写比较
#define strcasecmp strcmpi