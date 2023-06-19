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

    protected:
        std::string data;
    };
}// namespace sese::yaml