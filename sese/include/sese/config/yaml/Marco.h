/**
* @file Marco.h
* @author kaoru
* @version 0.1
* @brief YAML 操作宏
* @date 2023年9月13日
*/

#pragma once

#include <sese/Config.h>

#define SESE_YAML_SET(target, type, name, value)               \
    {                                                          \
        auto node = std::make_shared<sese::yaml::BasicData>(); \
        node->setDataAs<type>(value);                          \
        target->set(name, node);                               \
    }

#define SESE_YAML_PUT(target, type, value)                     \
    {                                                          \
        auto node = std::make_shared<sese::yaml::BasicData>(); \
        node->setDataAs<type>(value);                          \
        target->push(node);                                    \
    }

#define SESE_YAML_PUT_STRING(target, value)   \
    SESE_YAML_PUT(target, std::string, value) \
    SESE_MARCO_END

#define SESE_YAML_PUT_INTEGER(target, value) \
    SESE_YAML_PUT(target, int64_t, value)    \
    SESE_MARCO_END

#define SESE_YAML_PUT_BOOLEAN(target, value) \
    SESE_YAML_PUT(target, bool, value)       \
    SESE_MARCO_END

#define SESE_YAML_PUT_DOUBLE(target, value) \
    SESE_YAML_PUT(target, double, value)    \
    SESE_MARCO_END

#define SESE_YAML_PUT_NULL(target)                             \
    {                                                          \
        auto node = std::make_shared<sese::yaml::BasicData>(); \
        node->setNull(true);                                   \
        target->push(node);                                    \
    }                                                          \
    SESE_MARCO_END

#define SESE_YAML_SET_STRING(target, name, value)   \
    SESE_YAML_SET(target, std::string, name, value) \
    SESE_MARCO_END

#define SESE_YAML_SET_BOOLEAN(target, name, value) \
    SESE_YAML_SET(target, bool, name, value)       \
    SESE_MARCO_END

#define SESE_YAML_SET_INTEGER(target, name, value) \
    SESE_YAML_SET(target, int64_t, name, value)    \
    SESE_MARCO_END

#define SESE_YAML_SET_DOUBLE(target, name, value) \
    SESE_YAML_SET(target, double, name, value)    \
    SESE_MARCO_END

#define SESE_YAML_SET_NULL(target, name)                       \
    {                                                          \
        auto node = std::make_shared<sese::yaml::BasicData>(); \
        node->setNull(true);                                   \
        target->set(name, node);                               \
    }                                                          \
    SESE_MARCO_END

#define SESE_YAML_GET(value, target, type, name, def)               \
    {                                                               \
        auto node = target->getDataAs<sese::yaml::BasicData>(name); \
        if (node == nullptr) {                                      \
            value = def;                                            \
        } else {                                                    \
            value = node->getDataAs<type>(def);                     \
        }                                                           \
    }

#define SESE_YAML_GET_STRING(value, target, name, def)   \
    std::string value;                                   \
    SESE_YAML_GET(value, target, std::string, name, def) \
    SESE_MARCO_END

#define SESE_YAML_GET_BOOLEAN(value, target, name, def) \
    bool value;                                         \
    SESE_YAML_GET(value, target, bool, name, def)       \
    SESE_MARCO_END

#define SESE_YAML_GET_INTEGER(value, target, name, def) \
    int64_t value;                                      \
    SESE_YAML_GET(value, target, int64_t, name, def)    \
    SESE_MARCO_END

#define SESE_YAML_GET_DOUBLE(value, target, name, def) \
    double value;                                      \
    SESE_YAML_GET(value, target, double, name, def)    \
    SESE_MARCO_END
