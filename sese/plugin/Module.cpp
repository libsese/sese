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

#include "sese/plugin/Module.h"
#include "sese/plugin/Marco.h"

using getModuleInfoFunc = sese::plugin::ModuleInfo *();
using getFactoryFunc = sese::plugin::ClassFactory *();

#define STR1(R) #R
#define STR2(R) STR1(R)

sese::plugin::Module::Ptr sese::plugin::Module::open(const std::string &path) noexcept {
    auto obj = LibraryObject::create(path);
    if (!obj) {
        return nullptr;
    }

    auto get_module_info = (getModuleInfoFunc *) obj->findFunctionByName(STR2(GET_MODULE_INFO_FUNC_NAME));
    auto get_factory = (getFactoryFunc *) obj->findFunctionByName(STR2(GET_CLASS_FACTORY_FUNC_NAME));
    if (!get_module_info || !get_factory) {
        return nullptr;
    }

    auto info = get_module_info();
    auto factory = get_factory();
    // 按照标准，此步不应该出错
    // if (!info || !factory) {
    //    return nullptr;
    // }

    auto m = MAKE_UNIQUE_PRIVATE(Module);
    m->object = obj;
    m->info = info;
    m->factory = factory;
    m->factory->init();
    return m;
}

#undef STR2
#undef STR1

sese::Result<sese::plugin::Module::Ptr> sese::plugin::Module::openEx(const std::string &path) noexcept {
    if (auto result = open(path)) {
        return result;
    }
    return Result<Ptr>::fromLastError();
}

sese::plugin::Module::Ptr sese::plugin::Module::openWithPath(const system::Path &path) noexcept {
    return sese::plugin::Module::open(path.getNativePath());
}

sese::plugin::BaseClass::Ptr sese::plugin::Module::createClass(const std::string &id) noexcept {
    auto p_factory = (ClassFactory *) this->factory;
    return p_factory->createClassWithName(id);
}

const char *sese::plugin::Module::getName() noexcept {
    auto p_info = (ModuleInfo *) this->info;
    return p_info->moduleName;
}

const char *sese::plugin::Module::getVersion() noexcept {
    auto p_info = (ModuleInfo *) this->info;
    return p_info->versionString;
}

const char *sese::plugin::Module::getDescription() noexcept {
    auto p_info = (ModuleInfo *) this->info;
    return p_info->description;
}

const sese::plugin::ClassFactory::RegisterInfoMapType &sese::plugin::Module::getRegisterClassInfo() noexcept {
    return factory->getRegisterClassInfo();
}
