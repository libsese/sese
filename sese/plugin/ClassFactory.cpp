#include "sese/plugin/ClassFactory.h"

sese::plugin::ClassFactory::ClassFactory(const sese::plugin::ClassFactory::ParamListType &initializerList) noexcept {
    for (decltype(auto) pair : initializerList) {
        infoMap[pair.first] = pair.second;
    }
}

sese::plugin::BaseClass::Ptr sese::plugin::ClassFactory::createClassWithId(const std::string &id) noexcept {
    auto iterator = infoMap.find(id);
    if (iterator != infoMap.end()) {
        return iterator->second.builder();
    } else {
        return nullptr;
    }
}

const sese::plugin::ClassFactory::RegisterInfoMapType &sese::plugin::ClassFactory::getRegisterClassInfo() noexcept {
    return infoMap;
}
