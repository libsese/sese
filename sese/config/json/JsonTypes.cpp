#include <sese/config/json/JsonTypes.h>

using namespace sese::json;

Data::Data(DataType type) : TYPE(type) {}

BasicData::BasicData() noexcept
    : Data(DataType::BASIC),
      data("null"), _isNull(true) {}

BasicData::BasicData(std::string text) noexcept
    : Data(DataType::BASIC),
      data(std::move(text)),
      _isNull(false) {}

void BasicData::setNull(bool null) noexcept {
    _isNull = null;
    if (null) {
        data = "null";
    }
}

void BasicData::setNotNull(std::string new_data, bool is_string) noexcept {
    if (is_string) {
        data = "\"" + new_data + "\"";
    } else {
        data = std::move(new_data);
    }
    _isNull = false;
}

ObjectData::ObjectData() : Data(DataType::OBJECT) {}

void ObjectData::set(const std::string &key, const Data::Ptr &data) {
    keyValueSet[key] = data;
}

Data::Ptr ObjectData::get(const std::string &key) const noexcept {
    auto res = keyValueSet.find(key);
    if (res == keyValueSet.end()) {
        return nullptr;
    } else {
        return res->second;
    }
}

ArrayData::ArrayData() : Data(DataType::ARRAY) {}