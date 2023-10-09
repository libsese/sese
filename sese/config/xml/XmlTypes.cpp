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

void Element::setAttribute(const std::string &nameStr, const std::string &valueStr) {
    attributes[nameStr] = valueStr;
}

const std::string &Element::getAttribute(const std::string &nameStr, const std::string &defaultValue) const {
    auto iterator = attributes.find(nameStr);
    if (iterator == attributes.end()) {
        return defaultValue;
    } else {
        return iterator->second;
    }
}

void Element::removeAttribute(const std::string &nameStr) {
    attributes.erase(nameStr);
}

} // namespace sese::xml