/**
 * @file Config.h
 * @brief Windows 平台配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include <Windows.h>

#pragma warning(disable : 4819)

#if defined(_M_X64)
#define SESE_ARCH_X64
#elif defined(_M_ARM64)
#define SESE_ARCH_ARM64
#else
#error only support x86_64 & arm64
#endif

/// C++ 版本标识符
#define SESE_CXX_STANDARD _MSVC_LANG

#if defined(WINDOWS_DLL) && !defined(__clang__)
/// 可导出类型标识符
#define API __declspec(dllexport)
#else
/// 可导出类型标识符
#define API
#endif

#ifdef NEED_DBGHELP
#pragma comment(lib, "DbgHelp.lib")
#endif

/// pid_t 格式化占位符
#define PRIdTid "u"
/// 忽略大小写比较
#define strcasecmp strcmpi

#ifdef __clang__
#define SESE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define SESE_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

/// 进程 ID 标识符
using pid_t = DWORD;
/// 线程 ID 标识符
using tid_t = uint32_t;
/// Native Socket 类型
using socket_t = SOCKET;