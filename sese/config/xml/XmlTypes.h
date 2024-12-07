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

/**
 * @file XmlTypes.h
 * @brief XML Type Definitions
 * @author kaoru
 * @date October 23, 2022
 */

#pragma once
#include <sese/Config.h>
#include <vector>
#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::xml {

/// XML Element
class Element {
public:
    using Ptr = std::shared_ptr<Element>;
    using AttrType = std::map<std::string, std::string>;
    using SubType = std::vector<Element::Ptr>;

    explicit Element(const std::string &name);
    [[nodiscard]] const std::string &getName() const;
    [[nodiscard]] const std::string &getValue() const;
    void setValue(std::string var);
    [[nodiscard]] const AttrType &getAttributes() const;
    [[nodiscard]] const SubType &getElements() const;
    void addElement(const Element::Ptr &element);
    void setAttribute(const std::string &name, const std::string &value);
    [[nodiscard]] const std::string &getAttribute(const std::string &name, const std::string &default_value) const;
    void removeAttribute(const std::string &name);

    SubType elements;

private:
    std::string name;
    std::string value;
    AttrType attributes;
};
} // namespace sese::xml