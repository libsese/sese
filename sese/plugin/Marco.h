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
