/**
 * @file JsonTypes.h
 * @brief JSON 类型定义
 * @author kaoru
 * @date 2022年5月14日
 */

#pragma once

#include <sese/Config.h>

#include <map>
#include <list>

#ifdef _WIN32
#pragma warning(disable : 4251)
#else
#define _atoi64(val) strtoll(val, nullptr, 10)
#endif

namespace sese::json {

    // 从文本处理方面来看，可以归纳 4 种类型
    enum class DataType {
        // 基础数据类型，如字符串、数字等
        Basic,
        // 标准的 Json 对象
        Object,
        // 数组
        Array,
    };

    /// JSON 数据基类
    class API Data {
    public:
        using Ptr = std::shared_ptr<Data>;

        /**
     * 设置 Data 类型
     * @param type 类型
     */
        explicit Data(DataType type);
        virtual ~Data() = default;

        [[nodiscard]] DataType getType() const noexcept { return type; }

    protected:
        const DataType type;// 节点的数据类型 - 数据类型是不可变的
    };

    /// JSON 基础数据类型
    class API BasicData final : public Data {
    public:
        using Ptr = std::shared_ptr<BasicData>;

        explicit BasicData() noexcept;
        explicit BasicData(std::string text) noexcept;
        [[nodiscard]] const std::string &raw() const noexcept { return data; }

        /**
     * 设置值为 null
     */
        void setNull(bool null) noexcept;
        /**
     * 设置值为非 null
     * @param newData 新的数据
     * @param isString 是否为字符串
     * @warning 最好不要手动调用此函数，考虑使用 setDataAs() 函数
     */
        void setNotNull(std::string newData, bool isString = false) noexcept;

        template<typename T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, bool>, bool> t);

        template<typename T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> t);

        template<typename T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, std::string>, const std::string &> t);

        template<typename T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, double>, double> t);

        /**
     * 返回一个值，指示该数据是否为 null
     * @return
     */
        [[nodiscard]] bool isNull() const { return _isNull; }

        template<typename T>
        std::enable_if_t<std::is_same_v<T, std::string>, std::string>
        getDataAs(const T &defaultValue);

        template<typename T>
        std::enable_if_t<std::is_same_v<T, std::string_view>, std::string_view>
        getDataAs(const T &defaultValue);

        template<typename T>
        std::enable_if_t<std::is_same_v<T, int64_t>, int64_t>
        getDataAs(T defaultValue);

        template<typename T>
        std::enable_if_t<std::is_same_v<T, bool>, bool>
        getDataAs(T defaultValue = false);

        template<typename T>
        std::enable_if_t<std::is_same_v<T, double>, double>
        getDataAs(T defaultValue);

    protected:
        std::string data;
        bool _isNull;
    };

    template<typename T>
    std::enable_if_t<std::is_same_v<T, std::string>, std::string> BasicData::getDataAs(const T &defaultValue) {
        if (_isNull) {
            return defaultValue;
        } else {
            return data.substr(1, data.size() - 2);
        }
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, std::string_view>, std::string_view> BasicData::getDataAs(const T &defaultValue) {
        if (_isNull) {
            return defaultValue;
        } else {
            return {data.c_str() + 1, data.size() - 2};
        }
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> BasicData::getDataAs(T defaultValue) {
        if (_isNull) {
            return defaultValue;
        } else {
            return _atoi64(data.c_str());
        }
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, bool>, bool> BasicData::getDataAs(T defaultValue) {
        if (_isNull) return defaultValue;
        if (data == "true") {
            return true;
        } else {
            return false;
        }
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<T, double>, double> BasicData::getDataAs(T defaultValue) {
        if (_isNull) {
            return defaultValue;
        } else {
            char *pEnd = nullptr;
            auto res = std::strtod(data.c_str(), &pEnd);
            return res;
        }
    }

    template<typename T>
    void BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, bool>, bool> t) {
        if (t) {
            setNotNull("true");
        } else {
            setNotNull("false");
        }
    }

    template<typename T>
    void BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, double>, double> t) {
        data = std::to_string(t);
    }

    template<typename T>
    void BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> t) {
        data = std::to_string(t);
    }

    template<typename T>
    void BasicData::setDataAs(const std::enable_if_t<std::is_same_v<T, std::string>, const std::string &> t) {
        data = "\"" + t + "\"";
    }

    class ArrayData;

    /// JSON 对象数据类型
    class API ObjectData final : public Data {
    public:
        using Ptr = std::shared_ptr<ObjectData>;
        explicit ObjectData();

        [[nodiscard]] inline size_t size() const noexcept { return keyValueSet.size(); }
        [[nodiscard]] inline bool empty() const noexcept { return keyValueSet.empty(); }
        void set(const std::string &key, const Data::Ptr &data);
        [[nodiscard]] Data::Ptr get(const std::string &key) const noexcept;
        inline std::map<std::string, Data::Ptr>::iterator begin() noexcept { return keyValueSet.begin(); }
        inline std::map<std::string, Data::Ptr>::iterator end() noexcept { return keyValueSet.end(); }

        template<typename T>
        std::shared_ptr<std::enable_if_t<std::is_same_v<BasicData, T>, BasicData>>
        getDataAs(const std::string &key);

        template<typename T>
        std::shared_ptr<std::enable_if_t<std::is_same_v<ObjectData, T>, ObjectData>>
        getDataAs(const std::string &key);

        template<typename T>
        std::shared_ptr<std::enable_if_t<std::is_same_v<ArrayData, T>, ArrayData>>
        getDataAs(const std::string &key);

    protected:
        std::map<std::string, Data::Ptr> keyValueSet;
    };

    template<typename T>
    std::shared_ptr<std::enable_if_t<std::is_same_v<BasicData, T>, BasicData>> ObjectData::getDataAs(const std::string &key) {
        auto raw = get(key);
        if (raw == nullptr) {
            return nullptr;
        } else {
            if (raw->getType() == DataType::Basic) {
                return std::dynamic_pointer_cast<BasicData>(raw);
            } else {
                return nullptr;
            }
        }
    }

    template<typename T>
    std::shared_ptr<std::enable_if_t<std::is_same_v<ObjectData, T>, ObjectData>> ObjectData::getDataAs(const std::string &key) {
        auto raw = get(key);
        if (raw == nullptr) {
            return nullptr;
        } else {
            if (raw->getType() == DataType::Object) {
                return std::dynamic_pointer_cast<ObjectData>(raw);
            } else {
                return nullptr;
            }
        }
    }

    template<typename T>
    std::shared_ptr<std::enable_if_t<std::is_same_v<ArrayData, T>, ArrayData>> ObjectData::getDataAs(const std::string &key) {
        auto raw = get(key);
        if (raw == nullptr) {
            return nullptr;
        } else {
            if (raw->getType() == DataType::Array) {
                return std::dynamic_pointer_cast<ArrayData>(raw);
            } else {
                return nullptr;
            }
        }
    }

    /// JSON 数组数据类型
    class API ArrayData final : public Data {
    public:
        using Ptr = std::shared_ptr<ArrayData>;
        explicit ArrayData();

        [[nodiscard]] inline size_t size() const noexcept { return valueSet.size(); }
        [[nodiscard]] inline bool empty() const noexcept { return valueSet.empty(); }
        void push(const Data::Ptr &data) noexcept { valueSet.emplace_back(data); }
        inline std::list<Data::Ptr>::iterator begin() noexcept { return valueSet.begin(); }
        inline std::list<Data::Ptr>::iterator end() noexcept { return valueSet.end(); }

    protected:
        std::list<Data::Ptr> valueSet;
    };

}// namespace sese::json