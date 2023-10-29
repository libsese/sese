#include <sese\util\Value.h>

#include <cassert>

using sese::Value;

Value::Value(Type type) {
    switch (type) {
        case Type::Null:
            data.emplace<Null>();
            break;
        case Type::Bool:
            data.emplace<bool>(false);
            break;
        case Type::Int:
            data.emplace<int>(0);
            break;
        case Type::Double:
            data.emplace<double>(0.0);
            break;
        case Type::String:
            data.emplace<String>();
            break;
        case Type::List:
            data.emplace<List>();
            break;
        case Type::Dict:
            data.emplace<Dict>();
            break;
        case Type::Blob:
            data.emplace<Blob>();
            break;
    }
}

Value::Value(bool value) : data(value) {}
Value::Value(int value) : data(value) {}
Value::Value(double value) : data(value){}
Value::Value(const String &value) : data(value) {}

Value::Type Value::getType() const {
    return static_cast<Type>(data.index());
}

bool Value::isNull() const {
    return getType() == Type::Null;
}

bool Value::isBool() const {
    return getType() == Type::Bool;
}

bool Value::isInt() const {
    return getType() == Type::Int;
}

bool Value::isDouble() const {
    return getType() == Type::Double;
}

bool Value::isString() const {
    return getType() == Type::String;
}

std::optional<bool> Value::getIfBool() const {
    if (isBool()) {
        return std::get<bool>(data);
    }
    return std::nullopt;
}

std::optional<int> Value::getIfInt() const {
    if (isInt()) {
        return std::get<int>(data);
    }
    return std::nullopt;
}

std::optional<double> Value::getIfDouble() const {
    if (isDouble()) {
        return std::get<double>(data);
    }
    return std::nullopt;
}


std::optional<Value::String> Value::getIfString() const {
    if (isString()) {
        return std::get<String>(data);
    }
    return std::nullopt;
}

bool Value::getBool() const {
    assert(isBool());
    return std::get<bool>(data);
}

int Value::getInt() const {
    assert(isInt());
    return std::get<int>(data);
}

double Value::getDouble() const {
    assert(isDouble());
    return std::get<double>(data);
}

Value::String Value::getString() const {
    assert(isString());
    return std::get<String>(data);
}