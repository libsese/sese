/**
 * @file Marco.h
 * @brief 模块工具宏
 * @author kaoru
 * @version 1.0
 */

#pragma once

#include "sese/plugin/ModuleInfo.h"
#include "sese/Util.h"

#ifdef SESE_PLATFORM_WINDOWS
#define SESE_EXTERN extern "C" __declspec(dllexport)
#else
#define SESE_EXTERN extern "C"
#endif

#define GET_MODULE_INFO_FUNC_NAME getModuleInfo
#define GET_CLASS_FACTORY_FUNC_NAME getFactory

#define REGISTER_CLASS(type)                                                     \
    {                                                                            \
        auto t = &typeid(type);                                                  \
        auto real_name = sese::getClassName(t);                                  \
        this->infoMap[real_name] = {t, [] { return std::make_shared<type>(); }}; \
    }                                                                            \
    SESE_MARCO_END

#define DEFINE_CLASS_FACTORY(name, version, description)                    \
    class name final : public sese::plugin::ClassFactory {                  \
    public:                                                                 \
        name() : ClassFactory() {                                           \
        }                                                                   \
        void init() override;                                               \
    };                                                                      \
    SESE_EXTERN sese::plugin::ModuleInfo *GET_MODULE_INFO_FUNC_NAME() {     \
        static sese::plugin::ModuleInfo info{#name, version, description};  \
        return &info;                                                       \
    }                                                                       \
    SESE_EXTERN sese::plugin::ClassFactory *GET_CLASS_FACTORY_FUNC_NAME() { \
        static name factory;                                                \
        return &factory;                                                    \
    }                                                                       \
    void name::init()
