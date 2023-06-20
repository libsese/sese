#pragma once

#include "sese/Config.h"

#include <map>
#include <list>

namespace sese::yaml {

    enum class DataType {
        ObjectData,
        ArrayData,
        BasicData
    };

    class API Data {
    public:
        using Ptr = std::shared_ptr<Data>;

        explicit Data(DataType type) noexcept;
        virtual ~Data() noexcept = default;

        [[nodiscard]] DataType getType() const noexcept { return type; }

    private:
        const DataType type;
    };

    class ObjectData final : public Data {
    public:
        using Ptr = std::shared_ptr<ObjectData>;

        explicit ObjectData() noexcept;

        [[nodiscard]] inline size_t size() const noexcept { return keyValueSet.size(); }
        [[nodiscard]] inline bool empty() const noexcept { return keyValueSet.empty(); }
        void set(const std::string &key, const Data::Ptr &data) noexcept;
        [[nodiscard]] Data::Ptr get(const std::string &key) noexcept;
        inline std::map<std::string, Data::Ptr>::iterator begin() noexcept { return keyValueSet.begin(); }
        inline std::map<std::string, Data::Ptr>::iterator end() noexcept { return keyValueSet.end(); }

    protected:
        std::map<std::string, Data::Ptr> keyValueSet;
    };

    class ArrayData final : public Data {
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

    class BasicData final : public Data {
    public:
        using Ptr = std::shared_ptr<BasicData>;

        explicit BasicData() noexcept;

        template<class T>
        std::enable_if_t<std::is_same_v<T, std::string>>
        getDataAs(const T &def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                return data;
            }
        }

        template<class T>
        std::enable_if_t<std::is_same_v<T, std::string_view>>
        getDataAs(const T &def) const noexcept {
            if (_isNull) {
                return def;
            } else {
                return data;
            }
        }

        template<class T>
        std::enable_if_t<std::is_same_v<T, bool>>
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

        template<class T>
        std::enable_if_t<std::is_same_v<T, int64_t>>
        getDataAs(T def) const noexcept;

        template<class T>
        std::enable_if_t<std::is_same_v<T, double>>
        getDataAs(T def) const noexcept;

        [[nodiscard]] int64_t getDataAsTimestamp(int64_t def) const noexcept;

        [[nodiscard]] bool isNull() const noexcept { return _isNull; }

        template<class T>
        void
        setDataAs(const std::enable_if_t<std::is_same_v<T, std::string>> &value) noexcept {
            this->data = value;
        }

        template<class T>
        void
        setDataAs(const std::enable_if_t<std::is_same_v<T, std::string_view>> &value) noexcept {
            this->data = value;
        }

        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, bool>> value) noexcept {
            this->data = value ? "true" : "false";
        }

        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, int64_t>> value) noexcept {
            this->data = std::to_string(value);
        }

        template<class T>
        void
        setDataAs(std::enable_if_t<std::is_same_v<T, double>> value) noexcept {
            this->data = std::to_string(value);
        }

        void setNull(bool null) noexcept {
            this->_isNull = null;
        }

        void setDataAsTimestamp(int64_t value) noexcept;

    protected:
        std::string data;
        bool _isNull = false;
    };
}// namespace sese::yaml