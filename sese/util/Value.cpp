#include <sese/util/Value.h>
#include <sese/util/Util.h>

#include <algorithm>
#include <cassert>

using sese::Value;
using List = sese::Value::List;
using Dict = sese::Value::Dict;

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
Value::Value(double value) : data(value) {}
Value::Value(const char *value) : Value(std::string(value)) {}
Value::Value(const String &value) : data(value) {}
Value::Value(const Blob &value) : data(value) {}
Value::Value(const char *value, size_t length) {
    auto vector = std::vector<uint8_t>();
    vector.reserve(length);
    vector.insert(vector.end(), value, value + length);
    data.emplace<Blob>(std::move(vector));
}
Value::Value(List &&value) : data(value) {}
Value::Value(Dict &&value) : data(value) {}

Value Value::list() {
    return Value(Type::List);
}

Value Value::dict() {
    return Value(Type::Dict);
}

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

bool Value::isBlob() const {
    return getType() == Type::Blob;
}

bool Value::isList() const {
    return getType() == Type::List;
}

bool Value::isDict() const {
    return getType() == Type::Dict;
}

std::optional<bool> Value::getIfBool() const {
    if (isBool()) {
        return std::get<bool>(data);
    } else if (isInt()) {
        return std::get<int>(data) != 0;
    } else if (isDouble()) {
        return std::get<double>(data) != 0.0;
    } else if (isString()) {
        auto t = sese::strcmpDoNotCase(std::get<String>(data).c_str(), "true") == 0;
        if (t) {
            return t;
        }
        auto f = sese::strcmpDoNotCase(std::get<String>(data).c_str(), "false") == 0;
        if (f) {
            return f;
        }
        return std::nullopt;
    }
    return std::nullopt;
}

std::optional<int> Value::getIfInt() const {
    if (isInt()) {
        return std::get<int>(data);
    } else if (isDouble()) {
        return static_cast<int>(std::get<double>(data));
    }
    return std::nullopt;
}

std::optional<double> Value::getIfDouble() const {
    if (isInt()) {
        return static_cast<double>(std::get<int>(data));
    } else if (isDouble()) {
        return std::get<double>(data);
    }
    return std::nullopt;
}

Value::String *Value::getIfString() {
    return std::get_if<String>(&data);
}

Value::Blob *Value::getIfBlob() {
    return std::get_if<Blob>(&data);
}

Value::List *Value::getIfList() {
    return std::get_if<List>(&data);
}

Value::Dict *Value::getIfDict() {
    return std::get_if<Dict>(&data);
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

const Value::String &Value::getString() const {
    assert(isString());
    return std::get<String>(data);
}

Value::String &Value::getString() {
    assert(isString());
    return std::get<String>(data);
}

Value::Blob &Value::getBlob() {
    assert(isBlob());
    return std::get<Blob>(data);
}

const Value::Blob &Value::getBlob() const {
    assert(isBlob());
    return std::get<Blob>(data);
}

bool Value::operator==(const sese::Value &rhs) const {
    if (getType() != rhs.getType()) {
        return false;
    } else {
        switch (getType()) {
            case Type::Null:
                return true;
            case Type::Bool:
                return std::get<bool>(data) == std::get<bool>(rhs.data);
            case Type::Int:
                return std::get<int>(data) == std::get<int>(rhs.data);
            case Type::Double:
                return std::get<double>(data) == std::get<double>(rhs.data);
            case Type::String:
                return std::get<String>(data) == std::get<String>(rhs.data);
            case Type::Blob:
                return std::get_if<Blob>(&data) == std::get_if<Blob>(&rhs.data);
            case Type::List:
                return std::get_if<List>(&data) == std::get_if<List>(&rhs.data);
            case Type::Dict:
                return std::get_if<Dict>(&data) == std::get_if<Dict>(&rhs.data);
        }
        return false;
    }
}

bool Value::operator!=(const sese::Value &rhs) const {
    return !(*this == rhs);
}

size_t List::empty() const { return vector.empty(); }

size_t List::size() const { return vector.size(); }

void List::reserve(size_t size) { vector.reserve(size); }

void List::resize(size_t size) { vector.resize(size); }

void List::clear() { vector.clear(); }

const Value &List::operator[](size_t index) const { return vector[index]; }

Value &List::operator[](size_t index) { return vector[index]; }

size_t List::erase(const sese::Value &value) {
    size_t count = 0;
    vector.erase(
            std::remove_if(
                    vector.begin(),
                    vector.end(),
                    [&value, &count](const Value &v) {
                        if (v == value) {
                            count += 1;
                            return true;
                        } else {
                            return false;
                        }
                    }
            ),
            vector.end()
    );
    return count;
}

List::Iterator List::erase(Iterator it) { return vector.erase(std::move(it)); }

List::ConstIterator List::erase(ConstIterator it) { return vector.erase(std::move(it)); }

List::Iterator List::erase(Iterator first, Iterator last) {
    return vector.erase(std::move(first), std::move(last));
}

List::ConstIterator List::erase(ConstIterator first, ConstIterator last) {
    return vector.erase(std::move(first), std::move(last));
}

List::Iterator List::begin() { return vector.begin(); }

List::ConstIterator List::begin() const { return vector.begin(); }

List::Iterator List::end() { return vector.end(); }

List::ConstIterator List::end() const { return vector.end(); }

List::ConstIterator List::cbegin() const { return vector.cbegin(); }

List::ConstIterator List::cend() const { return vector.cend(); }

List::ReverseIterator List::rbegin() { return vector.rbegin(); }

List::ConstReverseIterator List::rbegin() const { return vector.rbegin(); }

List::ReverseIterator List::rend() { return vector.rend(); }

List::ConstReverseIterator List::rend() const { return vector.rend(); }

const Value &List::front() const {
    assert(!vector.empty());
    return vector.front();
}

Value &List::front() {
    assert(!vector.empty());
    return vector.front();
}

const Value &List::back() const {
    assert(!vector.empty());
    return vector.back();
}

Value &List::back() {
    assert(!vector.empty());
    return vector.back();
}

void List::append(Value &&value) & { vector.push_back(value); }

void List::append(bool value) & { vector.emplace_back(value); }

void List::append(int value) & { vector.emplace_back(value); }

void List::append(double value) & { vector.emplace_back(value); }

void List::append(const char *value) & { vector.emplace_back(value); }

void List::append(const String &value) & { vector.emplace_back(value); }

void List::append(const Blob &value) & { vector.emplace_back(value); }

void List::append(const char *value, size_t length) & { vector.emplace_back(value, length); }

void List::append(List &&value) & { vector.emplace_back(std::move(value)); }

void List::append(Dict &&value) & { vector.emplace_back(std::move(value)); }

List &&List::append(Value &&value) && {
    vector.emplace_back(std::move(value));
    return std::move(*this);
}

List &&List::append(bool value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(int value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(double value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(const char *value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(const String &value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(const Blob &value) && {
    vector.emplace_back(value);
    return std::move(*this);
}

List &&List::append(const char *value, size_t length) && {
    vector.emplace_back(value, length);
    return std::move(*this);
}

List &&List::append(List &&value) && {
    vector.emplace_back(std::move(value));
    return std::move(*this);
}

List &&List::append(Dict &&value) && {
    vector.emplace_back(std::move(value));
    return std::move(*this);
}

List::Iterator List::insert(sese::Value::List::Iterator it, sese::Value &&value) {
    return vector.insert(std::move(it), std::move(value));
}

bool Dict::empty() const {
    return map.empty();
}

size_t Dict::size() const {
    return map.size();
}

void Dict::clear() {
    map.clear();
}

bool Dict::contains(const Value::String &key) const {
    return map.count(key) > 0;
}

Dict::Iterator Dict::begin() { return map.begin(); }

Dict::ConstIterator Dict::begin() const { return map.begin(); }

Dict::Iterator Dict::end() { return map.end(); }

Dict::ConstIterator Dict::end() const { return map.end(); }

Dict::ConstIterator Dict::cbegin() const { return map.cbegin(); }

Dict::ConstIterator Dict::cend() const { return map.cend(); }

Dict::ReverseIterator Dict::rbegin() { return map.rbegin(); }

Dict::ConstReverseIterator Dict::rbegin() const { return map.rbegin(); }

Dict::ReverseIterator Dict::rend() { return map.rend(); }

Dict::ConstReverseIterator Dict::rend() const { return map.rend(); }

Dict::Iterator Dict::erase(Dict::Iterator it) {
    return map.erase(std::move(it));
}

Dict::Iterator Dict::erase(Dict::ConstIterator it) {
    return map.erase(std::move(it));
}

Value *Dict::find(const Value::String &key) {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second.get();
}

const Value *Dict::find(const Value::String &key) const {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second.get();
}

void Dict::set(const Value::String &key, Value &&value) & {
    //map[key] = std::make_unique<Value>(std::move(value));
}

void Dict::set(const Value::String &key, bool value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, int value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, double value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, const char *value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, const String &value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, const Blob &value) & {
    //map[key] = std::make_unique<Value>(value);
}

void Dict::set(const Value::String &key, const char *value, size_t length) & {
    //map[key] = std::make_unique<Value>(value, length);
}

void Dict::set(const Value::String &key, List &&value) & {
    //map[key] = std::make_unique<Value>(std::move(value));
}

void Dict::set(const Value::String &key, Dict &&value) & {
    //map[key] = std::make_unique<Value>(std::move(value));
}

Dict &&Dict::set(const Value::String &key, Value &&value) && {
    //map[key] = std::make_unique<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, bool value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, int value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, double value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const String &value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const Blob &value) && {
    //map[key] = std::make_unique<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value, size_t length) && {
    //map[key] = std::make_unique<Value>(value, length);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, List &&value) && {
    //map[key] = std::make_unique<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Dict &&value) && {
    //map[key] = std::make_unique<Value>(std::move(value));
    return std::move(*this);
}

bool Dict::remove(const Value::String &key) {
    return map.erase(key) > 0;
}