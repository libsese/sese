#include "sese/config/yaml/YamlTypes.h"

sese::yaml::Data::Data(sese::yaml::DataType type) noexcept
    : type(type) {
}

sese::yaml::ObjectData::ObjectData() noexcept
    : Data(DataType::ObjectData) {
}

sese::yaml::ArrayData::ArrayData() noexcept
    : Data(DataType::ArrayData) {
}

sese::yaml::BasicData::BasicData() noexcept
    : Data(DataType::BasicData) {
}

void sese::yaml::ObjectData::set(const std::string &key, const sese::yaml::Data::Ptr &data) noexcept {
    this->keyValueSet[key] = data;
}

sese::yaml::Data::Ptr sese::yaml::ObjectData::get(const std::string &key) noexcept {
    auto res = keyValueSet.find(key);
    if (res == keyValueSet.end()) {
        return nullptr;
    } else {
        return res->second;
    }
}