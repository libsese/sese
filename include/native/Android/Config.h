/**
 * @file Config.h
 * @brief Android 平台配置文件
 * @author kaoru
 * @date 2022年3月31日
*/
#pragma once

#if defined(__aarch64__)
#define SESE_ARCH_ARM64
#else
#error only support arm64
#endif

/// 可导出类型标识符
#define API
/// pid_t 格式化占位符
#define PRIdPid "d"
/// 忽略大小写比较
#define strcmpi strcasecmp

/// Native Socket 类型
using socket_t = int32_t;