/**
 * @file Config.h
 * @brief 通用配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <cassert>
#include <cinttypes>
#include <memory>
#include <string>
#ifdef _WIN32
#include "native/Windows/Config.h"
#elif __ANDROID__
#include "native/Android/Config.h"
#elif __linux__
#include "native/Linux/Config.h"
#elif __APPLE__
#include "native/Darwin/Config.h"
#endif

#pragma region Cpu 相关配置
constexpr static const char *VENDOR_INTEL_STRING = "GenuineIntel";
constexpr static const char *VENDOR_AMD_STRING = "AuthenticAMD";
#pragma endregion

#pragma region 异常配置
constexpr static const char *EXCEPTION_STRING = "Exception";
constexpr static const char *EXCEPTION_HEADER_STRING = "Exception: ";
constexpr static const char *EXCEPTION_LOGIC_STRING = "Logic exception";
constexpr static const char *EXCEPTION_INDEX_OUT_OF_BOUNDS_STRING = "Index out of bounds";
constexpr static const char *EXCEPTION_ILLEGAL_ARGUMENT_STRING = "Illegal argument exception";
constexpr static const char *EXCEPTION_IO_STRING = "IO exception";
#pragma endregion

#pragma region 日期时间配置
/// 默认时区
constexpr static int32_t TIME_DEFAULT_ZONE = 8;
/// 世界协调时匹配格式
constexpr static const char *TIME_DEFAULT_PATTERN = "UTCz yyyy-MM-dd HH:mm:ss.ff";
/// 格林威治时间匹配格式
constexpr static const char *TIME_GREENWICH_MEAN_PATTERN = "ddd, dd MMM yyyy HH:mm:ss %G%M%T";
/// 短格式
constexpr static const char *TIME_SHORT_PATTERN = "yyyy-MM-dd%THH:mm:ss.ff%Z";
#pragma endregion

#pragma region StringBuffer 相关配置
/// String Buffer 单元因子
constexpr static const size_t STRING_BUFFER_SIZE_FACTOR = 1024;
/// 空格字符
constexpr static const char *SPACE_CHARS = " \n\r\t\v\b";
#pragma endregion

#pragma region 线程配置
/// 默认线程名称
constexpr static const char *THREAD_DEFAULT_NAME = "UNKNOWN_THREAD";
/// 主线程名称
constexpr static const char *THREAD_MAIN_NAME = "Main";
#pragma endregion

#pragma region 日志配置
/// 主日志器名称
constexpr static const char *RECORD_MASTER_LOGGER_NAME = "ROOT";
/// 默认日志过滤器名称
constexpr static const char *RECORD_DEFAULT_FILTER = "DEFAULT_FILTER";
/// 日志输出缓存区大小
constexpr static const size_t RECORD_OUTPUT_BUFFER = 2048;
/// 日志格式化器时间匹配格式
constexpr static const char *RECORD_DEFAULT_TIME_PATTERN = TIME_SHORT_PATTERN;
/// 日志格式化器匹配格式
constexpr static const char *RECORD_DEFAULT_TEXT_PATTERN = "[%ln] %c %lv %fi %tn:%th> %m";
#pragma endregion

#pragma region Stream 配置
/// 字节流缓冲区单元因子
constexpr static const size_t STREAM_BYTE_STREAM_SIZE_FACTOR = 1024;
#pragma endregion