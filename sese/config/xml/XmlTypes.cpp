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