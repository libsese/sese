#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <optional>

namespace sese {

class Value {
public:
    enum class Type {
        Null = 0,
        Bool,
        Int,
        Double,
        String,
        Blob,
        List,
        Dict
    };

    using String = std::string;
    using Blob = std::vector<uint8_t>;

    class Null {};

    class List {
    public:
        using Raw = std::vector<Value>;

    private:
        Raw vector;
    };

    class Dict {
    public:
        using Raw = std::map<std::string, std::unique_ptr<Value>>;

    private:
        Raw map;
    };

    Value() = default;
    explicit Value(Type type);
    explicit Value(bool value);
    explicit Value(int value);
    explicit Value(double value);
    explicit Value(const String &value);

    [[nodiscard]] Type getType() const;

    [[nodiscard]] bool isNull() const;
    [[nodiscard]] bool isBool() const;
    [[nodiscard]] bool isInt() const;
    [[nodiscard]] bool isDouble() const;
    [[nodiscard]] bool isString() const;

    [[nodiscard]] std::optional<bool> getIfBool() const;
    [[nodiscard]] std::optional<int> getIfInt() const;
    [[nodiscard]] std::optional<double> getIfDouble() const;
    [[nodiscard]] std::optional<String> getIfString() const;

    [[nodiscard]] bool getBool() const;
    [[nodiscard]] int getInt() const;
    [[nodiscard]] double getDouble() const;
    [[nodiscard]] String getString() const;

private:
    std::variant<Null, bool, int, double, String, Blob, List, Dict> data;
};
} // namespace sese