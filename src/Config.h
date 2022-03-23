#pragma once
#include <cstdint>

#ifdef _WIN32
#ifdef WINDOWS_DLL
#define API __declspec(dllexport)
#else
#define API
#endif
#endif

#ifdef __linux__
#include <cassert>
#define API
#endif

// 异常
#define EXCEPTION_STRING "Exception"
#define EXCEPTION_HEADER_STRING "Exception: "
#define LOGIC_STRING "Logic exception"
#define INDEX_OUT_OF_BOUNDS_STRING "index out of bounds"

// 默认时区
#define DEFAULT_TIME_ZONE 8
// String Buffer 单元因子
#define STRING_BUFFER_SIZE_FACTOR 1024
// 默认线程名称
#define DEFAULT_THREAD_NAME "UNKNOWN_THREAD"
// 主线程名称
#define MAIN_THREAD_NAME "Main"
// 主日志器名称
#define MASTER_LOGGER_NAME "ROOT"
// 日志输出缓存区大小
#define LOGGER_OUTPUT_BUFFER 2048
// 格林威治时间匹配格式
#define GREENWICH_MEAN_TIME_PATTERN "ddd, dd MMM yyyy HH:mm:ss %G%M%T"
// 时间匹配格式
#define DEFAULT_TIME_PATTERN "UTCz yyyy-MM-dd HH:mm:ss.ff"
// 日志格式化器时间匹配格式
#define DEFAULT_RECORD_TIME_PATTERN "yyyy-MM-dd HH:mm:ss.ff"
// 日志格式化器匹配格式
#define DEFAULT_RECORD_TEXT_PATTERN "[%ln] %c %lv %tn:%th %f:%li> %m"
// 空格字符
#define SPACE_CHARS " \n\r\t\v\b"
// Windows 下需要调试器 - 不需要可关
#define NEED_DBGHELP 1