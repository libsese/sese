/**
 * @file Config.h
 * @brief 通用配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <cassert>
#include <cinttypes>
#include <string>
#include <sese/Memory.h>
#ifdef _WIN32
#include "sese/native/Windows/Config.h"
#elif __linux__
#include "sese/native/Linux/Config.h"
#elif __APPLE__
#include "sese/native/Darwin/Config.h"
#endif
#undef assert

/// 默认时区
constexpr static int32_t TIME_DEFAULT_ZONE = 8;
/// 世界协调时匹配格式
constexpr static const char *TIME_DEFAULT_PATTERN = "UTCz yyyy-MM-dd HH:mm:ss.ff";
/// 格林威治时间匹配格式
constexpr static const char *TIME_GREENWICH_MEAN_PATTERN = "ddd, dd MMM yyyy HH:mm:ss %G%M%T";
/// 短格式
constexpr static const char *TIME_SHORT_PATTERN = "yyyy-MM-dd%THH:mm:ss.ff%Z";

/// String Buffer 单元因子
constexpr static const size_t STRING_BUFFER_SIZE_FACTOR = 1024;
/// 空格字符
constexpr static const char *SPACE_CHARS = " \n\r\t\v\b";

/// 默认线程名称
constexpr static const char *THREAD_DEFAULT_NAME = "UNKNOWN_THREAD";
/// 主线程名称
constexpr static const char *THREAD_MAIN_NAME = "Main";

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

/// 字节流缓冲区单元因子
constexpr static const size_t STREAM_BYTE_STREAM_SIZE_FACTOR = 1024;

/// 是否启用调试模式
constexpr static const bool ENABLE_TEST = true;

/// TCP Server 默认线程池大小
constexpr static const size_t SERVER_DEFAULT_THREADS = 4;
/// TCP Server 默认监听队列大小
constexpr static const size_t SERVER_MAX_CONNECTION = 511;

/// Http Util 单行最大大小
constexpr static const size_t HTTP_MAX_SINGLE_LINE = 1023;

/// Http Server 名称
constexpr static const char *HTTPD_NAME = "sese::http::HttpServer";
/// Http Keep-Alive 时长（单位：秒）
constexpr static const size_t HTTPD_KEEP_ALIVE_DURATION = 60;