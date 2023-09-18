/**
* @file XmlTypes.h
* @brief XML 类型定义
* @author kaoru
* @date 2022年10月23日
*/
#pragma once
#include <sese/Config.h>
#include <vector>
#include <map>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese::xml {

    /// XML 元素对象
    class API Element {
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
        [[nodiscard]] const std::string &getAttribute(const std::string &name, const std::string &defaultValue) const;
        void removeAttribute(const std::string &name);

        SubType elements;
    private:
        std::string name;
        std::string value;
        AttrType attributes;

    };
}// namespace sese::xml