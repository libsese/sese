/**
 * \file Marco.h
 * \version 0.1
 * \author kaoru
 * \brief RPC 服务相关宏定义
 */
#pragma once

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

#define SetExitCode(code)                                                              \
    {                                                                                  \
        auto _exit = result->getDataAs<sese::json::BasicData>(SESE_RPC_TAG_EXIT_CODE); \
        _exit->setDataAs<int64_t>(code);                                               \
    }

#define Get(value, args, result, type, name, def)                 \
    {                                                             \
        auto node = args->getDataAs<sese::json::BasicData>(name); \
        if (nullptr != node) {                                    \
            value = node->getDataAs<type>(def);                   \
        } else {                                                  \
            SetExitCode(SESE_RPC_CODE_ILLEGAL_ARGS);              \
            return;                                               \
        }                                                         \
    }

#define GetBoolean(value, name, def) \
    bool value;                      \
    Get(value, args, result, bool, name, def)

#define GetInteger(value, name, def) \
    int64_t value;                   \
    Get(value, args, result, int64_t, name, def)

#define GetDouble(value, name, def) \
    double value;                   \
    Get(value, args, result, double, name, def)

#define GetString(value, name, def) \
    std::string value;              \
    Get(value, args, result, std::string, name, def)