/**
 * @file Config.h
 * @brief Windows 平台配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#include <Windows.h>

#ifdef WINDOWS_DLL
#define API __declspec(dllexport)
#else
#define API
#endif

#ifdef NEED_DBGHELP
#pragma comment(lib, "DbgHelp.lib")
#endif

using pid_t = uint32_t;
#define PRIdPid "u"

#define strcasecmp strcmpi