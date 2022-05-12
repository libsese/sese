#pragma once
#include <sese/Config.h>
#include <map>
#include <list>

#ifdef _WIN32
#pragma warning(disable : 4251)
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

    // setter
    void setNull() noexcept;
    void setNotNull(std::string newData) noexcept;
    // getter
    [[nodiscard]] bool isNull() const { return _isNull; }

    template<typename T>
    const std::enable_if_t<std::is_same_v<T, std::string>> &
    getDataAs(T &defaultValue);

    template<typename T>
    std::enable_if_t<std::is_same_v<T, int64_t>>
    getDataAs(T &defaultValue);

    template<typename T>
    std::enable_if_t<std::is_same_v<T, bool>>
    getDataAs(T &defaultValue = false);

    template<typename T>
    std::enable_if_t<std::is_same_v<T, double>>
    getDataAs(T &defaultValue);

protected:
    std::string data;
    bool _isNull;
};

template<typename T>
const std::enable_if_t<std::is_same_v<T, std::string>> &sese::json::BasicData::getDataAs(T &defaultValue) {
    if (_isNull) {
        return defaultValue;
    } else {
        return data;
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, int64_t>> sese::json::BasicData::getDataAs(T &defaultValue) {
    if (_isNull) {
        return defaultValue;
    } else {
        return _atoi64(data.c_str());
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, bool>> sese::json::BasicData::getDataAs(T &defaultValue) {
    if (_isNull) return defaultValue;
    if (strcmp(data.c_str(), "true") == 0) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
std::enable_if_t<std::is_same_v<T, double>> sese::json::BasicData::getDataAs(T &defaultValue) {
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

class API sese::json::ObjectData : public sese::json::Data {
public:
    explicit ObjectData();

    [[nodiscard]] inline size_t size() const noexcept { return keyValueSet.size(); }
    [[nodiscard]] inline bool empty() const noexcept { return keyValueSet.empty(); }
    void set(const std::string& key, const Data::Ptr &data);
    [[nodiscard]] Data::Ptr get(const std::string &key) const noexcept;

protected:
    std::map<std::string, Data::Ptr> keyValueSet;
};

class API sese::json::ArrayData : public sese::json::Data {
public:
    explicit ArrayData();

    [[nodiscard]] inline size_t size() const noexcept { return valueSet.size(); }
    [[nodiscard]] inline bool empty() const noexcept { return valueSet.empty(); }
    void push(const Data::Ptr &data) noexcept { valueSet.emplace_back(data); }
    inline std::list<Data::Ptr>::iterator begin() noexcept { return valueSet.begin(); }
    inline std::list<Data::Ptr>::iterator end() noexcept { return valueSet.end(); }

protected:
    std::list<Data::Ptr> valueSet;
};