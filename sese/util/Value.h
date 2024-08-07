/// \file Value.h
/// \brief 值容器类，设计参考 https://github.com/chromium/chromium/blob/main/base/values.h
/// \author kaoru
/// \date 2023年10月29日

#pragma once

#include <map>
#include <variant>
#include <vector>
#include <optional>

#include <sese/Config.h>
#include <sese/text/StringBuilder.h>
#include <sese/text/Format.h>

namespace sese {

/// \brief 值容器类
class Value {
public:
    enum class Type {
        NONE = 0,
        BOOL,
        INT,
        DOUBLE,
        STRING,
        BLOB,
        LIST,
        DICT
    };

    using Integer = int64_t;
    using String = std::string;
    using Blob = std::vector<uint8_t>;

    class List;
    class Dict;

    /// \brief 空值容器类型
    class Null {};

    /// \brief 列表值容器类型
    class List {
    public:
        using Raw = std::vector<Value>;
        using Iterator = Raw::iterator;
        using ConstIterator = Raw::const_iterator;
        using ReverseIterator = Raw::reverse_iterator;
        using ConstReverseIterator = Raw::const_reverse_iterator;

        [[nodiscard]] size_t empty() const;
        [[nodiscard]] size_t size() const;
        void reserve(size_t size);
        void resize(size_t size);
        void clear();

        const Value &operator[](size_t index) const;
        Value &operator[](size_t index);

        size_t erase(const Value &value);
        Iterator erase(Iterator it);
        ConstIterator erase(ConstIterator it);
        Iterator erase(const Iterator &first, const Iterator &last);
        ConstIterator erase(const ConstIterator &first, const ConstIterator &last);

        Iterator begin();
        [[nodiscard]] ConstIterator begin() const;
        Iterator end();
        [[nodiscard]] ConstIterator end() const;

        [[nodiscard]] ConstIterator cbegin() const;
        [[nodiscard]] ConstIterator cend() const;

        ReverseIterator rbegin();
        [[nodiscard]] ConstReverseIterator rbegin() const;
        ReverseIterator rend();
        [[nodiscard]] ConstReverseIterator rend() const;

        [[nodiscard]] const Value &front() const;
        Value &front();
        [[nodiscard]] const Value &back() const;
        Value &back();

        void append(Value &&value) &;
        void append(bool value) &;
        void append(Integer value) &;
        void append(double value) &;
        void append(const char *value) &;
        void append(String &&value) &;
        void append(Blob &&value) &;
        void append(const char *bytes, size_t length) &;
        void append(List &&value) &;
        void append(Dict &&value) &;

        List &&append(Value &&value) &&;
        List &&append(bool value) &&;
        List &&append(Integer value) &&;
        List &&append(double value) &&;
        List &&append(const char *value) &&;
        List &&append(String &&value) &&;
        List &&append(const char *bytes, size_t length) &&;
        List &&append(Blob &&value) &&;
        List &&append(List &&value) &&;
        List &&append(Dict &&value) &&;

        Iterator insert(Iterator it, Value &&value);

    private:
        Raw vector;
    };

    /// \brief 字典值容器类型
    class Dict {
    public:
        using Raw = std::map<String, Value *>;
        using Iterator = Raw::iterator;
        using ConstIterator = Raw::const_iterator;
        using ReverseIterator = Raw::reverse_iterator;
        using ConstReverseIterator = Raw::const_reverse_iterator;

        Dict() = default;
        ~Dict();
        // move constructor
        Dict(Dict &&other) noexcept;
        Dict &operator=(Dict &&other) noexcept;
        // copy constructor
        Dict(const Dict &other) = delete;
        Dict &operator=(const Dict &other) = delete;

        [[nodiscard]] bool empty() const;
        [[nodiscard]] size_t size() const;
        void clear();
        [[nodiscard]] bool contains(const String &key) const;

        Iterator begin();
        [[nodiscard]] ConstIterator begin() const;
        Iterator end();
        [[nodiscard]] ConstIterator end() const;

        [[nodiscard]] ConstIterator cbegin() const;
        [[nodiscard]] ConstIterator cend() const;

        ReverseIterator rbegin();
        [[nodiscard]] ConstReverseIterator rbegin() const;
        ReverseIterator rend();
        [[nodiscard]] ConstReverseIterator rend() const;

        Iterator erase(const Iterator &it);
        Iterator erase(const ConstIterator &it);

        [[nodiscard]] const Value *find(const String &key) const;
        Value *find(const String &key);

        void set(const String &key, Value &&value) &;
        void set(const String &key, bool value) &;
        void set(const String &key, Integer value) &;
        void set(const String &key, double value) &;
        void set(const String &key, const char *value) &;
        void set(const String &key, const char *value, size_t length) &;
        void set(const String &key, String &&value) &;
        void set(const String &key, Blob &&value) &;
        void set(const String &key, List &&value) &;
        void set(const String &key, Dict &&value) &;

        Dict &&set(const String &key, Value &&value) &&;
        Dict &&set(const String &key, bool value) &&;
        Dict &&set(const String &key, Integer value) &&;
        Dict &&set(const String &key, double value) &&;
        Dict &&set(const String &key, const char *value) &&;
        Dict &&set(const String &key, const char *value, size_t length) &&;
        Dict &&set(const String &key, String &&value) &&;
        Dict &&set(const String &key, Blob &&value) &&;
        Dict &&set(const String &key, List &&value) &&;
        Dict &&set(const String &key, Dict &&value) &&;

        bool remove(const String &key);

    private:
        Raw map;
    };

    Value() = default;
    explicit Value(Type type);
    /// bool
    explicit Value(bool value);
    /// Integer
    explicit Value(Integer value);
    /// double
    explicit Value(double value);
    /// string
    explicit Value(const char *value);
    /// blob
    explicit Value(const char *bytes, size_t length);
    /// string
    explicit Value(String &&value) noexcept;
    /// blob
    explicit Value(Blob &&value);
    /// list
    explicit Value(List &&value);
    /// dict
    explicit Value(Dict &&value);

    static Value list();
    static Value dict();

    [[nodiscard]] Type getType() const;

    [[nodiscard]] bool isNull() const;
    [[nodiscard]] bool isBool() const;
    [[nodiscard]] bool isInt() const;
    [[nodiscard]] bool isDouble() const;
    [[nodiscard]] bool isString() const;
    [[nodiscard]] bool isBlob() const;
    [[nodiscard]] bool isList() const;
    [[nodiscard]] bool isDict() const;

    /// 允许数值类型转换为布尔型
    /// \return 布尔型
    [[nodiscard]] std::optional<bool> getIfBool() const;
    /// 允许从数数值类型转换成整形
    /// \return 整形
    [[nodiscard]] std::optional<Integer> getIfInt() const;
    /// 允许从数值类型转换成双精度浮点型
    /// \return 双精度浮点型
    [[nodiscard]] std::optional<double> getIfDouble() const;
    [[nodiscard]] String *getIfString();
    [[nodiscard]] Blob *getIfBlob();
    [[nodiscard]] Dict *getIfDict();
    [[nodiscard]] List *getIfList();

    [[nodiscard]] bool getBool() const;
    [[nodiscard]] Integer getInt() const;
    [[nodiscard]] double getDouble() const;
    [[nodiscard]] const String &getString() const;
    [[nodiscard]] String &getString();
    [[nodiscard]] const Blob &getBlob() const;
    [[nodiscard]] Blob &getBlob();
    [[nodiscard]] const List &getList() const;
    [[nodiscard]] List &getList();
    [[nodiscard]] const Dict &getDict() const;
    [[nodiscard]] Dict &getDict();

    [[nodiscard]] std::string toString(size_t level = 4) const noexcept;

    bool operator==(const Value &rhs) const;
    bool operator!=(const Value &rhs) const;

    void toString(text::StringBuilder &string_builder, size_t level) const noexcept;

private:
    std::variant<Null, bool, Integer, double, String, Blob, List, Dict> data;
};

template<>
struct text::overload::Formatter<Value> {
    size_t level = 4;

    bool parse(const std::string &args) {
        char *end;
        level = static_cast<size_t>(std::strtol(args.c_str(), &end, 10));
        if (level == 0) {
            return false;
        }
        return true;
    }

    void format(FmtCtx &ctx, Value &value) const {
        value.toString(ctx.builder, level);
    }
};

} // namespace sese