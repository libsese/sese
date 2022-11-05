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

#define GetBoolean4Server(value, name, def) \
    bool value;                             \
    Get4Server(value, args, result, bool, name, def)

#define GetInteger4Server(value, name, def) \
    int64_t value;                          \
    Get4Server(value, args, result, int64_t, name, def)

#define GetDouble4Server(value, name, def) \
    double value;                          \
    Get4Server(value, args, result, double, name, def)

#define GetString4Server(value, name, def) \
    std::string value;                     \
    Get4Server(value, args, result, std::string, name, def)

#define Set(target, type, name, value)                         \
    {                                                          \
        auto node = std::make_shared<sese::json::BasicData>(); \
        node->setDataAs<type>(value);                          \
        target->set(name, node);                               \
    }

#define SetBoolean(target, name, value) \
    Set(target, bool, name, value)

#define SetInteger(target, name, value) \
    Set(target, int64_t, name, value)

#define SetDouble(target, name, value) \
    Set(target, double, name, value)

#define SetString(target, name, value) \
    Set(target, std::string, name, value)

#define Get(value, target, type, name, def)                         \
    {                                                               \
        auto node = target->getDataAs<sese::json::BasicData>(name); \
        if (nullptr == node) {                                      \
            value = def;                                            \
        } else {                                                    \
            value = node->getDataAs<type>(def);                     \
        }                                                           \
    }

#define GetBoolean(value, target, name, def) \
    bool value;                              \
    Get(value, target, bool, name, def)

#define GetInteger(value, target, name, def) \
    int64_t value;                           \
    Get(value, target, int64_t, name, def)

#define GetDouble(value, target, name, def) \
    double value;                           \
    Get(value, target, double, name, def)

#define GetString(value, target, name, def) \
    std::string value;                      \
    Get(value, target, std::string, name, def)