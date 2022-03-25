#pragma once
#include <cassert>
#include <cinttypes>
#ifdef _WIN32
#include "native/win/Config.h"
#endif
#ifdef __linux__
#include "native/linux/Config.h"
#endif
#ifdef __APPLE__
#include "native/darwin/Config.h"
#endif

#pragma region 异常配置
#define EXCEPTION_STRING  "Exception"
#define EXCEPTION_HEADER_STRING  "Exception: "
#define EXCEPTION_LOGIC_STRING  "Logic exception"
#define EXCEPTION_INDEX_OUT_OF_BOUNDS_STRING  "Index out of bounds"
#define EXCEPTION_ILLEGAL_ARGUMENT_STRING "Illegal argument exception"
#pragma endregion

#pragma region 日期时间配置
// 默认时区
#define TIME_DEFAULT_ZONE 8
// 世界协调时匹配格式
#define TIME_DEFAULT_PATTERN  "UTCz yyyy-MM-dd HH:mm:ss.ff"
// 格林威治时间匹配格式
#define TIME_GREENWICH_MEAN_PATTERN  "ddd, dd MMM yyyy HH:mm:ss %G%M%T"
// 短格式
#define TIME_SHORT_PATTERN  "yyyy-MM-dd%THH:mm:ss.ff%Z"
#pragma endregion

#pragma region StringBuffer 相关配置
// String Buffer 单元因子
#define STRING_BUFFER_SIZE_FACTOR 1024
// 空格字符
#define SPACE_CHARS  " \n\r\t\v\b"
#pragma endregion

#pragma region 线程配置
// 默认线程名称
#define THREAD_DEFAULT_NAME  "UNKNOWN_THREAD"
// 主线程名称
#define THREAD_MAIN_NAME  "Main"
#pragma endregion

#pragma region 日志配置
// 主日志器名称
#define RECORD_MASTER_LOGGER_NAME  "ROOT"
// 默认日志过滤器名称
#define RECORD_DEFAULT_FILTER  "DEFAULT_FILTER"
// 日志输出缓存区大小
#define RECORD_OUTPUT_BUFFER 2048
// 日志格式化器时间匹配格式
#define RECORD_DEFAULT_TIME_PATTERN  TIME_SHORT_PATTERN
// 日志格式化器匹配格式
#define RECORD_DEFAULT_TEXT_PATTERN  "[%ln] %c %lv %fi %tn:%th %fn:%li> %m"
#pragma endregion