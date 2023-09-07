/// \file YamlTypes.h
/// \brief 描述 yaml 基本类型
/// \author kaoru
/// \version 0.1
/// \date 2023年6月30日

#pragma once

#include "sese/Config.h"

#include <map>
#include <list>

#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#pragma warning(disable : 4251)
#endif

namespace sese::yaml {

    /// 类型枚举
    enum class DataType {
        ObjectData,
        ArrayData,
        BasicData
    };

    class ObjectData;
    class ArrayData;
    class BasicData;

    /// 类型基类
    class API Data {
    public:
        using Ptr = std::shared_ptr<Data>;

        explicit Data(DataType type) noexcept;
        virtual ~Data() noexcept = default;

        [[nodiscard]] DataType getType() const noexcept { return type; }

    private:
        const DataType type;
    };

    /// 对象类型
    class API ObjectData final : public Data {
    public:
        using Ptr = std::shared_ptr<ObjectData>;

        explicit ObjectData() noexcept;

        [[nodiscard]] inline size_t size() const noexcept { return keyValueSet.size(); }
        [[nodiscard]] inline bool empty() const noexcept { return keyValueSet.empty(); }
        void set(const std::string &key, const Data::Ptr &data) noexcept;
        [[nodiscard]] Data::Ptr get(const std::string &key) noexcept;
        inline std::map<std::string, Data::Ptr>::iterator begin() noexcept { return keyValueSet.begin(); }
        inline std::map<std::string, Data::Ptr>::iterator end() noexcept { return keyValueSet.end(); }

        template<class T>
        std::enable_if_t<std::is_same_v<T, ObjectData>, std::shared_ptr<sese::yaml::ObjectData>>
        getDataAs(const std::string &key) noexcept;

        template<class T>
        std::enable_if_t<std::is_same_v<T, ArrayData>, std::shared_ptr<sese::yaml::ArrayData>>
        getDataAs(const std::string &key) noexcept;

        template<class T>
        std::enable_if_t<std::is_same_v<T, BasicData>, std::shared_ptr<sese::yaml::BasicData>>
        getDataAs(const std::string &key) noexcept;

    protected:
        std::map<std::string, Data::Ptr> keyValueSet;
    };

    /// 数组类型
    class API ArrayData final : public Data {
    public:
        using Ptr = std::shared_ptr<ArrayData>;

        explicit ArrayData() noexcept;
        [[nodiscard]] inline size_t size() const noexcept { return valueSet.size(); }
        [[nodiscard]] inline bool empty() const noexcept { return valueSet.empty(); }
        void push(const Data::Ptr &data) noexcept { valueSet.emplace_back(data); }
        inline std::list<Data::Ptr>::iterator begin() noexcept { return valueSet.begin(); }
        inline std::list<Data::Ptr>::iterator end() noexcept { return valueSet.end(); }

    protected:
        std::list<Data::Ptr> valueSet;
    };

    /// 基本数据类
    class API BasicData final : public Data {
    public:
        using Ptr = std::shared_ptr<BasicData>;

        explicit BasicData() noexcept;

        /// 将数据作为 std::string 类型获取
        /// \tparam T std::string
        /// \param def 该值不存在时返回的默认值
        /// \return 结果
        template<class T>
        std::enable_if_t<std::is_same_v<T, std::string>, std::string>
        getDataAs(const T &def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                return data;
            }
        }

        /// 将数据作为 std::string_view 类型获取
        /// \tparam T std::string_view
        /// \param def 该值不存在时返回的默认值
        /// \return 结果
        template<class T>
        std::enable_if_t<std::is_same_v<T, std::string_view>, std::string_view>
        getDataAs(const T &def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                return data;
            }
        }

        /// 将数据作为 bool 类型获取
        /// \tparam T bool
        /// \param def 该值不存在时返回的默认值
        /// \return 结果
        template<class T>
        std::enable_if_t<std::is_same_v<T, bool>, bool>
        getDataAs(T def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                if (0 == strcasecmp(data.c_str(), "yes") || 0 == strcasecmp(data.c_str(), "true")) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        /// 将数据作为整型获取
        /// \tparam T int64_t
        /// \param def 该值不存在时返回的默认值
        /// \return 结果
        template<class T>
        std::enable_if_t<std::is_same_v<T, int64_t>, int64_t>
        getDataAs(T def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                char *end;
                return std::strtol(data.c_str(), &end, 10);
            }
        }

        /// 将数据作为浮点型获取
        /// \tparam T double
        /// \param def 该值不存在时返回的默认值
        /// \return 结果
        template<class T>
        std::enable_if_t<std::is_same_v<T, double>, double>
        getDataAs(T def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                char *end;
                return std::strtod(data.c_str(), &end);
            }
        }

        /// \return 当前值是否为空
        [[nodiscard]] bool isNull() const noexcept { return _isNull; }

        /// 将值作为 std::string 填入
        /// \tparam T std::string
        /// \param value 值
        template<class T>
        void
        setDataAs(const std::enable_if_t<std::is_same_v<T, std::string>, std::string> &value) noexcept {
            this->data = value;
        }

        /// 将值作为 std::string_view 填入
        /// \tparam T std::string_view
        /// \param value 值
        template<class T>
        void
        setDataAs(const std::enable_if_t<std::is_same_v<T, std::string_view>, std::string_view> &value) noexcept {
            this->data = value;
        }

        /// 将值作为 bool 类型填入
        /// \tparam T bool
        /// \param value 值
        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, bool>, bool> value) noexcept {
            this->data = value ? "true" : "false";
        }

        /// 将当前值作为整型填入
        /// \tparam T int64_t
        /// \param value 值
        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> value) noexcept {
            this->data = std::to_string(value);
        }

        /// 将当前值作为浮点型填入
        /// \tparam T double
        /// \param value 值
        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, double>, double> value) noexcept {
            this->data = std::to_string(value);
        }

        /// 设置当前值是否为空
        /// \param null 是否为空
        void setNull(bool null) noexcept {
            this->_isNull = null;
        }

    protected:
        std::string data;
        bool _isNull = false;
    };
}// namespace sese::yaml

template<class T>
std::enable_if_t<std::is_same_v<T, sese::yaml::ObjectData>, std::shared_ptr<sese::yaml::ObjectData>> sese::yaml::ObjectData::getDataAs(const std::string &key) noexcept {
    auto p = get(key);
    if (p != nullptr && p->getType() == DataType::ObjectData) {
        return std::dynamic_pointer_cast<ObjectData>(p);
    } else {
        return nullptr;
    }
}

template<class T>
std::enable_if_t<std::is_same_v<T, sese::yaml::ArrayData>, std::shared_ptr<sese::yaml::ArrayData>> sese::yaml::ObjectData::getDataAs(const std::string &key) noexcept {
    auto p = get(key);
    if (p != nullptr && p->getType() == DataType::ArrayData) {
        return std::dynamic_pointer_cast<ArrayData>(p);
    } else {
        return nullptr;
    }
}

template<class T>
std::enable_if_t<std::is_same_v<T, sese::yaml::BasicData>, std::shared_ptr<sese::yaml::BasicData>> sese::yaml::ObjectData::getDataAs(const std::string &key) noexcept {
    auto p = get(key);
    if (p != nullptr && p->getType() == DataType::BasicData) {
        return std::dynamic_pointer_cast<BasicData>(p);
    } else {
        return nullptr;
    }
}