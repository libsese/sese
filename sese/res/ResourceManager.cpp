#include <sese/res/ResourceManager.h>

sese::res::Resource::Ptr sese::res::ResourceManager::getResource(const std::string &name) const {
    auto iterator = resourceMap.find(name);
    if (iterator == resourceMap.end()) {
        return nullptr;
    }
    return iterator->second;
}