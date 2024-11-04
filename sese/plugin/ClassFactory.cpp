#include "sese/plugin/ClassFactory.h"

sese::plugin::BaseClass::Ptr sese::plugin::ClassFactory::createClassWithName(const std::string &name) noexcept {
    auto iterator = infoMap.find(name);
    if (iterator != infoMap.end()) {
        return iterator->second.builder();
    }
    return nullptr;
}

sese::Result<sese::plugin::BaseClass::Ptr> sese::plugin::ClassFactory::createClassWithNameEx(const std::string &name) noexcept {
    if (auto result = createClassWithName(name)) {
        return result;
    }
    return {1, "Cannot find class named " + name};
}

const sese::plugin::ClassFactory::RegisterInfoMapType &sese::plugin::ClassFactory::getRegisterClassInfo() noexcept {
    return infoMap;
}
