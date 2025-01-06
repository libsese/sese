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

/**
 * @file Config.h
 * @brief Generic profiles
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include <cassert>
#include <cinttypes>
#include <algorithm>
#include <string>
#include <sese/util/Memory.h>
#include <sese/system/Environment.h>

// namespace alias
namespace sese {
namespace sys = sese::system;
}

#ifdef _WIN32
#pragma warning(disable : 4819)
#define SESE_PLATFORM_WINDOWS
#include "WindowsConfig.h"
#elif __linux__
#define SESE_PLATFORM_LINUX
#include "LinuxConfig.h"
#elif __APPLE__
#define SESE_PLATFORM_APPLE
#include "DarwinConfig.h"
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

/// Default time zone
static constexpr int32_t TIME_DEFAULT_ZONE = 8;
/// UTC Match format
static constexpr const char *TIME_DEFAULT_PATTERN = "UTCz yyyy-MM-dd HH:mm:ss.ff";
/// Greenwich Mean Time Matching Format
static constexpr const char *TIME_GREENWICH_MEAN_PATTERN = "ddd, dd MMM yyyy HH:mm:ss %G%M%T";
/// Time short format
static constexpr const char *TIME_SHORT_PATTERN = "yyyy-MM-dd%THH:mm:ss.ff%Z";

/// String Buffer Unit factor
static constexpr size_t STRING_BUFFER_SIZE_FACTOR = 1024;
/// Whitespace characters
static constexpr const char *SPACE_CHARS = " \n\r\t\v\b";

/// Default thread name
static constexpr const char *THREAD_DEFAULT_NAME = "UNKNOWN";
/// The name of the main thread
static constexpr const char *THREAD_MAIN_NAME = "Main";

/// The size of the log output buffer
static constexpr size_t RECORD_OUTPUT_BUFFER = 4096;
/// Log formatter time matching format
#ifndef SESE_RECORD_TIME_PATTERN
static constexpr const char *RECORD_DEFAULT_TIME_PATTERN = TIME_SHORT_PATTERN;
#else
static constexpr const char *RECORD_DEFAULT_TIME_PATTERN = SESE_RECORD_TIME_PATTERN;
#endif
/// The log formatter matches the format
#ifndef SESE_RECORD_TEXT_PATTERN
static constexpr const char *RECORD_DEFAULT_TEXT_PATTERN = "c lv fn:li tn:th> m\n";
#else
static constexpr const char *RECORD_DEFAULT_TEXT_PATTERN = SESE_RECORD_TEXT_PATTERN;
#endif
/// The log output file name matches the pattern
static constexpr const char *RECORD_DEFAULT_FILE_TIME_PATTERN = "yyyyMMdd HHmmssff";
/// Asynchronous logger buffer size
static constexpr const size_t RECORD_BUFFER_SIZE = 4096;

/// Byte stream buffer unit factor
static constexpr const size_t STREAM_BYTE_STREAM_SIZE_FACTOR = 1024;

/// Whether to enable debug mode
#ifdef SESE_IS_DEBUG
static constexpr bool ENABLE_TEST = true;
#else
static constexpr bool ENABLE_TEST = false;
#endif

/// The number of timer time wheels
static constexpr size_t TIMER_WHEEL_NUMBER = 30;

/// TCP Server default thread pool size
static constexpr size_t SERVER_DEFAULT_THREADS = 4;
/// TCP Server defaults to the listener queue size
static constexpr size_t SERVER_MAX_CONNECTION = 511;
/// TCP Server Keep-Alive duration
static constexpr size_t SERVER_KEEP_ALIVE_DURATION = 10;

/// The maximum size of a single HTTP util
static constexpr size_t HTTP_MAX_SINGLE_LINE = 1023;
/// Http Server name
static constexpr const char *HTTPD_NAME = "sese::service::HttpServer";
/// HTTP multi-segment request boundary
static constexpr const char *HTTPD_BOUNDARY = "000000SESE0BOUNDARY000000";
/// Http Client name
static constexpr const char *HTTP_CLIENT_NAME = "sese::net::http::HttpClient";

/// Compressor Chunk default size
static constexpr size_t ZLIB_CHUNK_SIZE = 1024 * 16;

/// The size of the WSABUF carried by the IOCP when delivering the Recv event
static constexpr size_t IOCP_WSABUF_SIZE = 65535;

/// Framework default MTU value
static constexpr size_t MTU_VALUE = 1472;
/// Framework DNS packet size
static constexpr size_t DNS_PACKAGE_SIZE = 512;