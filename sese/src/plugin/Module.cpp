#include "sese/plugin/Module.h"
#include "sese/plugin/Marco.h"

using getModuleInfoFunc = sese::plugin::ModuleInfo *();
using getFactoryFunc = sese::plugin::ClassFactory *();

#define STR1(R) #R
#define STR2(R) STR1(R)

sese::plugin::Module::Ptr sese::plugin::Module::open(const std::string &path) noexcept {
    auto obj = LibraryLoader::open(path);
    if (!obj) {
        return nullptr;
    }

    auto getModuleInfo = (getModuleInfoFunc *) obj->findFunctionByName(STR2(GET_MODULE_INFO_FUNC_NAME));
    auto getFactory = (getFactoryFunc *) obj->findFunctionByName(STR2(GET_CLASS_FACTORY_FUNC_NAME));
    if (!getModuleInfo || !getFactory) {
        return nullptr;
    }

    auto info = getModuleInfo();
    auto factory = getFactory();
    if (!info || !factory) {
        return nullptr;
    }

    auto m = new Module;
    m->object = obj;
    m->info = info;
    m->factory = factory;
    return std::unique_ptr<Module>(m);
}

#undef STR2
#undef STR1

sese::plugin::BaseClass::Ptr sese::plugin::Module::createClassWithId(const std::string &id) noexcept {
    auto pFactory = (ClassFactory *) this->factory;
    return pFactory->createClassWithId(id);
}

const char *sese::plugin::Module::getName() noexcept {
    auto pInfo = (ModuleInfo *) this->info;
    return pInfo->moduleName;
}

const char *sese::plugin::Module::getVersion() noexcept {
    auto pInfo = (ModuleInfo *) this->info;
    return pInfo->versionString;
}

const char *sese::plugin::Module::getDescription() noexcept {
    auto pInfo = (ModuleInfo *) this->info;
    return pInfo->description;
}

const sese::plugin::ClassFactory::RegisterInfoMapType &sese::plugin::Module::getRegisterClassInfo() noexcept {
    return factory->getRegisterClassInfo();
}
