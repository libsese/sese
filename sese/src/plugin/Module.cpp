#include "sese/plugin/Module.h"
#include "sese/plugin/ModuleInfo.h"
#include "sese/plugin/BaseClassFactory.h"

using getModuleInfoFunc = sese::plugin::ModuleInfo *();
using getFactoryFunc = sese::plugin::BaseClassFactory *();

sese::plugin::Module::Ptr sese::plugin::Module::open(const std::string &path) noexcept {
    auto obj = LibraryLoader::open(path);
    if (!obj) {
        return nullptr;
    }

    auto getModuleInfo = (getModuleInfoFunc *) obj->findFunctionByName("getModuleInfo");
    auto getFactory = (getFactoryFunc *) obj->findFunctionByName("getFactory");
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

sese::plugin::BaseClass::Ptr sese::plugin::Module::createClassWithId(const std::string &id) noexcept {
    auto pFactory = (BaseClassFactory *) this->factory;
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