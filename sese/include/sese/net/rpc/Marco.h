/**
 * \file Marco.h
 * \version 0.2
 * \author kaoru
 * \brief RPC 服务相关宏定义
 */
#pragma once

#include "sese/config/json/Marco.h"

#define SESE_RPC_VERSION_0_1 "v0.1"

#define SESE_RPC_TAG_VERSION "rpc-ver"
#define SESE_RPC_TAG_NAME "rpc-name"
#define SESE_RPC_TAG_ARGS "rpc-args"
#define SESE_RPC_TAG_EXIT_CODE "rpc-exit-code"

#define SESE_RPC_CODE_SUCCEED 0
#define SESE_RPC_CODE_NONSUPPORT_VERSION 1
#define SESE_RPC_CODE_MISSING_REQUIRED_FIELDS 2
#define SESE_RPC_CODE_NO_EXIST_FUNC 3
#define SESE_RPC_CODE_ILLEGAL_ARGS 4

#define SESE_RPC_VALUE_UNDEF "undef"
#define SESE_RPC_VALUE_UNKNOWN "unknown error"
#define SESE_RPC_VALUE_SUCCEED "succeed"
#define SESE_RPC_VALUE_NONSUPPORT_VERSION "nonsupport version"
#define SESE_RPC_VALUE_MISSING_REQUIRED_FIELDS "missing required fields"
#define SESE_RPC_VALUE_NO_EXIST_FUNC "not found function"
#define SESE_RPC_VALUE_ILLEGAL_ARGS "illegal arguments"

/// 服务端设置退出码
/// \param code 退出码
#define SetExitCode(code)                                                              \
    {                                                                                  \
        auto _exit = result->getDataAs<sese::json::BasicData>(SESE_RPC_TAG_EXIT_CODE); \
        _exit->setDataAs<int64_t>(code);                                               \
    }

#define Get4Server(value, args, result, type, name, def)          \
    {                                                             \
        auto node = args->getDataAs<sese::json::BasicData>(name); \
        if (nullptr != node) {                                    \
            value = node->getDataAs<type>(def);                   \
        } else {                                                  \
            SetExitCode(SESE_RPC_CODE_ILLEGAL_ARGS);              \
            return;                                               \
        }                                                         \
    }

/// 获取 bool 类型的字段（服务端用）
/// \param value 变量名称
/// \param name 字段名称
/// \param def 默认值
#define GetBoolean4Server(value, name, def) \
    bool value;                             \
    Get4Server(value, args, result, bool, name, def) SESE_MARCO_END

/// 获取 int64_t 类型的字段（服务端用）
/// \param value 变量名称
/// \param name 字段名称
/// \param def 默认值
#define GetInteger4Server(value, name, def) \
    int64_t value;                          \
    Get4Server(value, args, result, int64_t, name, def) SESE_MARCO_END

/// 获取 double 类型的字段（服务端用）
/// \param value 变量名称
/// \param name 字段名称
/// \param def 默认值
#define GetDouble4Server(value, name, def) \
    double value;                          \
    Get4Server(value, args, result, double, name, def) SESE_MARCO_END

/// 获取 std::string 类型的字段（服务端用）
/// \param value 变量名称
/// \param name 字段名称
/// \param def 默认值
#define GetString4Server(value, name, def) \
    std::string value;                     \
    Get4Server(value, args, result, std::string, name, def) SESE_MARCO_END