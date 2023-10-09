/**
 * @file Marco.h
 * @brief 模块工具宏
 * @author kaoru
 * @version 0.2
 */

#pragma once

#include "sese/plugin/ModuleInfo.h"

#ifdef _WIN32
#define SESE_EXTERN extern "C" __declspec(dllexport)
#else
#define SESE_EXTERN extern "C"
#endif

#define GET_MODULE_INFO_FUNC_NAME getModuleInfo

#define GET_CLASS_FACTORY_FUNC_NAME getFactory

#define DEFINE_MODULE_INFO(name, version, description)                    \
    SESE_EXTERN sese::plugin::ModuleInfo *GET_MODULE_INFO_FUNC_NAME() {   \
        static sese::plugin::ModuleInfo info{name, version, description}; \
        return &info;                                                     \
    }

#define REGISTER_CLASS(id, class)                                      \
    std::pair<std::string, sese::plugin::ClassFactory::RegisterInfo> { \
        id,                                                            \
                sese::plugin::ClassFactory::RegisterInfo {             \
            &typeid(class),                                            \
                    []() -> sese::plugin::BaseClass::Ptr {             \
                        return std::make_shared<class>();              \
                    }                                                  \
        }                                                              \
    }

#define DEFINE_CLASS_FACTORY(...)                                           \
    SESE_EXTERN sese::plugin::ClassFactory *GET_CLASS_FACTORY_FUNC_NAME() { \
        static sese::plugin::ClassFactory factory({__VA_ARGS__});           \
        return &factory;                                                    \
    }
