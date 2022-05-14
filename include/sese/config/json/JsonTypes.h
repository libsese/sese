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
    /// Json 数据类型
    enum class DataType;

    /// Json 数据
    class API Data;
    /// 基础数据
    class API BasicData;
    /// Json 对象
    class API ObjectData;
    /// 数组
    class API ArrayData;
}// namespace sese::json

// 从文本处理方面来看，可以归纳 4 种类型
enum class sese::json::DataType {
    // 基础数据类型，如字符串、数字等
    Basic,
    // 标准的 Json 对象
    Object,
    // 数组
    Array,
};

class API sese::json::Data {
public:
    using Ptr = std::shared_ptr<Data>;

    explicit Data(DataType type);
    virtual ~Data() = default;

    [[nodiscard]] const std::string &getName() const noexcept { return name; }
    void setName(const std::string &newName) noexcept { this->name = newName; }
    [[nodiscard]] DataType getType() const noexcept { return type; }

protected:
    std::string name;   // 节点名称
    const DataType type;// 节点的数据类型 - 数据类型是不可变的
};

class API sese::json::BasicData : public sese::json::Data {
public:
    using Ptr = std::shared_ptr<BasicData>;

    explicit BasicData() noexcept;
    explicit BasicData(std::string text) noexcept;
    [[nodiscard]] const std::string &raw() const noexcept { return data; }

    // setter
    void setNull() noexcept;
    // isString 为 true 会自动处理引号
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

    // getter
    [[nodiscard]] bool isNull() const { return _isNull; }

    template<typename T>
    const std::enable_if_t<std::is_same_v<T, std::string>, std::string> &
    getDataAs(T &defaultValue);

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
const std::enable_if_t<std::is_same_v<T, std::string>, std::string> &sese::json::BasicData::getDataAs(T &defaultValue) {
    if (_isNull) {
        return defaultValue;
    } else {
        return data.substr(1, data.size() - 2);
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> sese::json::BasicData::getDataAs(T defaultValue) {
    if (_isNull) {
        return defaultValue;
    } else {
        return _atoi64(data.c_str());
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, bool>, bool> sese::json::BasicData::getDataAs(T defaultValue) {
    if (_isNull) return defaultValue;
    if (data == "true") {
        return true;
    } else {
        return false;
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, double>, double> sese::json::BasicData::getDataAs(T defaultValue) {
    if (_isNull) {
        return defaultValue;
    } else {
        char *pEnd = nullptr;
        auto res = strtod(data.c_str(), &pEnd);
        if (pEnd != nullptr) {
            return defaultValue;
        } else {
            return res;
        }
    }
}

template<typename T>
void sese::json::BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, bool>, bool> t) {
    if (t) {
        setNotNull("true");
    } else {
        setNotNull("false");
    }
}

template<typename T>
void sese::json::BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, double>, double> t) {
    data = std::to_string(t);
}

template<typename T>
void sese::json::BasicData::setDataAs(std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> t) {
    data = std::to_string(t);
}

template<typename T>
void sese::json::BasicData::setDataAs(const std::enable_if_t<std::is_same_v<T, std::string>, const std::string &> t) {
    data = std::move(t);
}

class API sese::json::ObjectData : public sese::json::Data {
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
    std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::BasicData, T>, sese::json::BasicData>>
    getDataAs(const std::string &key);

    template<typename T>
    std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::ObjectData, T>, sese::json::ObjectData>>
    getDataAs(const std::string &key);

    template<typename T>
    std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::ArrayData, T>, sese::json::ArrayData>>
    getDataAs(const std::string &key);

protected:
    std::map<std::string, Data::Ptr> keyValueSet;
};

template<typename T>
std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::BasicData, T>, sese::json::BasicData>> sese::json::ObjectData::getDataAs(const std::string &key) {
    auto raw = get(key);
    if (raw == nullptr) {
        return nullptr;
    } else {
        if (raw->getType() == DataType::Basic) {
            return std::dynamic_pointer_cast<sese::json::BasicData>(raw);
        } else {
            return nullptr;
        }
    }
}

template<typename T>
std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::ObjectData, T>, sese::json::ObjectData>> sese::json::ObjectData::getDataAs(const std::string &key) {
    auto raw = get(key);
    if (raw == nullptr) {
        return nullptr;
    } else {
        if (raw->getType() == DataType::Object) {
            return std::dynamic_pointer_cast<sese::json::ObjectData>(raw);
        } else {
            return nullptr;
        }
    }
}

template<typename T>
std::shared_ptr<std::enable_if_t<std::is_same_v<sese::json::ArrayData, T>, sese::json::ArrayData>> sese::json::ObjectData::getDataAs(const std::string &key) {
    auto raw = get(key);
    if (raw == nullptr) {
        return nullptr;
    } else {
        if (raw->getType() == DataType::Array) {
            return std::dynamic_pointer_cast<sese::json::ArrayData>(raw);
        } else {
            return nullptr;
        }
    }
}

class API sese::json::ArrayData : public sese::json::Data {
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