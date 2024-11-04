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

#include <sese/config/xml/XmlTypes.h>

namespace sese::xml {

Element::Element(const std::string &name) {
    this->name = name;
}

const Element::AttrType &Element::getAttributes() const {
    return attributes;
}

const Element::SubType &Element::getElements() const {
    return elements;
}

const std::string &Element::getName() const {
    return name;
}

const std::string &Element::getValue() const {
    return value;
}

void Element::setValue(std::string var) {
    value = std::move(var);
}

void Element::addElement(const Element::Ptr &element) {
    elements.push_back(element);
}

void Element::setAttribute(const std::string &name, const std::string &value) {
    attributes[name] = value;
}

const std::string &Element::getAttribute(const std::string &name, const std::string &default_value) const {
    if (const auto ITERATOR = attributes.find(name); ITERATOR == attributes.end()) {
        return default_value;
    } else {
        return ITERATOR->second;
    }
}

void Element::removeAttribute(const std::string &name) {
    attributes.erase(name);
}

} // namespace sese::xml