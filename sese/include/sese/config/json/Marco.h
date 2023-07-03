#pragma once

#include "sese/Config.h"

#define SESE_JSON_GET(value, target, type, name, def)               \
    {                                                               \
        auto node = target->getDataAs<sese::json::BasicData>(name); \
        if (nullptr == node) {                                      \
            value = def;                                            \
        } else {                                                    \
            value = node->getDataAs<type>(def);                     \
        }                                                           \
    }

/// 获取 bool 类型的字段
/// \param value 变量名称
/// \param target 父对象
/// \param name 字段名称
/// \param def 默认值
#define SESE_JSON_GET_BOOLEAN(value, target, name, def) \
    bool value;                                         \
    SESE_JSON_GET(value, target, bool, name, def)       \
    SESE_MARCO_END

/// 获取 int64_t 类型的字段
/// \param value 变量名称
/// \param target 父对象
/// \param name 字段名称
/// \param def 默认值
#define SESE_JSON_GET_INTEGER(value, target, name, def) \
    int64_t value;                                      \
    SESE_JSON_GET(value, target, int64_t, name, def)    \
    SESE_MARCO_END

/// 获取 double 类型的字段
/// \param value 变量名称
/// \param target 父对象
/// \param name 字段名称
/// \param def 默认值
#define SESE_JSON_GET_DOUBLE(value, target, name, def) \
    double value;                                      \
    SESE_JSON_GET(value, target, double, name, def)    \
    SESE_MARCO_END

/// 获取 std::string 类型的字段
/// \param value 变量名称
/// \param target 父对象
/// \param name 字段名称
/// \param def 默认值
#define SESE_JSON_GET_STRING(value, target, name, def)   \
    std::string value;                                   \
    SESE_JSON_GET(value, target, std::string, name, def) \
    SESE_MARCO_END

#define SESE_JSON_SET(target, type, name, value)               \
    {                                                          \
        auto node = std::make_shared<sese::json::BasicData>(); \
        node->setDataAs<type>(value);                          \
        target->set(name, node);                               \
    }

/// 设置一个 bool 类型的字段
/// \param target 父对象
/// \param name 字段名称
/// \param value 字段值
#define SESE_JSON_SET_BOOLEAN(target, name, value) SESE_JSON_SET(target, bool, name, value) SESE_MARCO_END

/// 设置一个 bool 类型的字段
/// \param target 父对象
/// \param name 字段名称
/// \param value 字段值
#define SESE_JSON_SET_INTEGER(target, name, value) SESE_JSON_SET(target, int64_t, name, value) SESE_MARCO_END

/// 设置一个 bool 类型的字段
/// \param target 父对象
/// \param name 字段名称
/// \param value 字段值
#define SESE_JSON_SET_DOUBLE(target, name, value) SESE_JSON_SET(target, double, name, value) SESE_MARCO_END

/// 设置一个 bool 类型的字段
/// \param target 父对象
/// \param name 字段名称
/// \param value 字段值
#define SESE_JSON_SET_STRING(target, name, value) SESE_JSON_SET(target, std::string, name, value) SESE_MARCO_END

/// 设置一个空的字段
/// \param target 父对象
/// \param name 字段名称
#define SESE_JSON_SET_NULL(target, name)                       \
    {                                                          \
        auto node = std::make_shared<sese::json::BasicData>(); \
        node->setNull(true);                                   \
        target->set(name, node);                               \
    }                                                          \
    SESE_MARCO_END

#define SESE_JSON_PUT(target, type, value)                     \
    {                                                          \
        auto node = std::make_shared<sese::json::BasicData>(); \
        node->setDataAs<type>(value);                          \
        target->push(node);                                    \
    }

#define SESE_JSON_PUT_STRING(target, value)   \
    SESE_JSON_PUT(target, std::string, value) \
    SESE_MARCO_END

#define SESE_JSON_PUT_BOOLEAN(target, value) \
    SESE_JSON_PUT(target, bool, value)       \
    SESE_MARCO_END

#define SESE_JSON_PUT_INTEGER(target, value) \
    SESE_JSON_PUT(target, int64_t, value)    \
    SESE_MARCO_END

#define SESE_JSON_PUT_DOUBLE(target, value) \
    SESE_JSON_PUT(target, double, value)    \
    SESE_MARCO_END

#define SESE_JSON_PUT_NULL(target)                             \
    {                                                          \
        auto node = std::make_shared<sese::json::BasicData>(); \
        node->setNull(true);                                   \
        target->push(node);                                    \
    }