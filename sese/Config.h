/**
 * @file Config.h
 * @brief 通用配置文件
 * @author kaoru
 * @date 2022年3月28日
 */

// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cassert>
#include <cinttypes>
#include <algorithm>
#include <string>
#include <sese/util/Memory.h>

#ifdef _WIN32
#pragma warning(disable : 4819)
#define SESE_PLATFORM_WINDOWS
#include "sese/native/win/Config.h"
#elif __linux__
#define SESE_PLATFORM_LINUX
#include "sese/native/linux/Config.h"
#elif __APPLE__
#define SESE_PLATFORM_APPLE
#include "sese/native/darwin/Config.h"
#endif

#ifdef assert
#undef assert
#endif

#define SESE_MARCO_END \
    switch (0)         \
    case 0:            \
    default:           \
        break

#define SESE_STD_WRAPPER(name, member) \
    [[nodiscard]] auto name() const { return member.name(); }

#if defined(__MINGW32__) || defined(__GNUC__)
#define SESE_DEPRECATED __attribute__((deprecated))
#define SESE_DEPRECATED_WITH(x) SESE_DEPRECATED
#else
#define SESE_DEPRECATED [[deprecated]]
#define SESE_DEPRECATED_WITH(x) [[deprecated(x)]]
#endif /* __MINGW32__ */

#if defined(_MSC_VER)
#define SESE_ALWAYS_INLINE __forceinline
#elif defined(__clang__)
#define SESE_ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(__GNUC__)
#define SESE_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#endif

/// 默认时区
static constexpr int32_t TIME_DEFAULT_ZONE = 8;
/// 世界协调时匹配格式
static constexpr const char *TIME_DEFAULT_PATTERN = "UTCz yyyy-MM-dd HH:mm:ss.ff";
/// 格林威治时间匹配格式
static constexpr const char *TIME_GREENWICH_MEAN_PATTERN = "ddd, dd MMM yyyy HH:mm:ss %G%M%T";
/// 短格式
static constexpr const char *TIME_SHORT_PATTERN = "yyyy-MM-dd%THH:mm:ss.ff%Z";

/// String Buffer 单元因子
static constexpr size_t STRING_BUFFER_SIZE_FACTOR = 1024;
/// 空格字符
static constexpr const char *SPACE_CHARS = " \n\r\t\v\b";

/// 默认线程名称
static constexpr const char *THREAD_DEFAULT_NAME = "UNKNOWN";
/// 主线程名称
static constexpr const char *THREAD_MAIN_NAME = "Main";

/// 日志输出缓存区大小
static constexpr size_t RECORD_OUTPUT_BUFFER = 4096;
/// 日志格式化器时间匹配格式
#ifndef SESE_RECORD_TIME_PATTERN
static constexpr const char *RECORD_DEFAULT_TIME_PATTERN = TIME_SHORT_PATTERN;
#else
static constexpr const char *RECORD_DEFAULT_TIME_PATTERN = SESE_RECORD_TIME_PATTERN;
#endif
/// 日志格式化器匹配格式
#ifndef SESE_RECORD_TEXT_PATTERN
static constexpr const char *RECORD_DEFAULT_TEXT_PATTERN = "c lv fn:li tn:th> m\n";
#else
static constexpr const char *RECORD_DEFAULT_TEXT_PATTERN = SESE_RECORD_TEXT_PATTERN;
#endif
/// 日志输出文件名称匹配模式
static constexpr const char *RECORD_DEFAULT_FILE_TIME_PATTERN = "yyyyMMdd HHmmssff";
/// 异步日志器缓冲区大小
static constexpr const size_t RECORD_BUFFER_SIZE = 4096;

/// 字节流缓冲区单元因子
static constexpr const size_t STREAM_BYTE_STREAM_SIZE_FACTOR = 1024;

/// 是否启用调试模式
#ifdef SESE_IS_DEBUG
static constexpr bool ENABLE_TEST = true;
#else
static constexpr bool ENABLE_TEST = false;
#endif

/// 定时器时间轮轮片数
static constexpr size_t TIMER_WHEEL_NUMBER = 30;

/// TCP Server 默认线程池大小
static constexpr size_t SERVER_DEFAULT_THREADS = 4;
/// TCP Server 默认监听队列大小
static constexpr size_t SERVER_MAX_CONNECTION = 511;
/// TCP Server Keep-Alive 时长
static constexpr size_t SERVER_KEEP_ALIVE_DURATION = 10;

/// Http Util 单行最大大小
static constexpr size_t HTTP_MAX_SINGLE_LINE = 1023;
/// Http Server 名称
static constexpr const char *HTTPD_NAME = "sese::service::HttpServer";
/// Http 多段请求 boundary
static constexpr const char *HTTPD_BOUNDARY = "000000SESE0BOUNDARY000000";
/// Http Client 名称
static constexpr const char *HTTP_CLIENT_NAME = "sese::net::http::HttpClient";

/// Compressor Chunk 默认大小
static constexpr size_t ZLIB_CHUNK_SIZE = 1024 * 16;

/// IOCP 投递的 Recv 事件时携带的 WSABUF 大小
static constexpr size_t IOCP_WSABUF_SIZE = 65535;

/// 框架默认 MTU 值
static constexpr size_t MTU_VALUE = 1472;
/// 框架 DNS 数据包大小
static constexpr size_t DNS_PACKAGE_SIZE = 512;