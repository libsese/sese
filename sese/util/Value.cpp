#include <sese/util/Value.h>
#include <sese/record/Marco.h>

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
Value::Value(const char *bytes, size_t length) {
    auto vector = std::vector<uint8_t>();
    vector.reserve(length);
    vector.insert(vector.end(), bytes, bytes + length);
    data.emplace<Blob>(std::move(vector));
}
Value::Value(String &&value) noexcept : data(std::move(value)) {}
Value::Value(Blob &&value) : data(std::move(value)) {}
Value::Value(List &&value) : data(std::move(value)) {}
Value::Value(Dict &&value) : data(std::move(value)) {}

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

const Value::List &Value::getList() const {
    assert(isList());
    return std::get<List>(data);
}

Value::List &Value::getList() {
    assert(isList());
    return std::get<List>(data);
}


const Value::Dict &Value::getDict() const {
    assert(isDict());
    return std::get<Dict>(data);
}

Value::Dict &Value::getDict() {
    assert(isDict());
    return std::get<Dict>(data);
}

// GCOVR_EXCL_START

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

// GCOVR_EXCL_STOP

std::string Value::toString(size_t level) const noexcept {
    level = std::max<size_t>(level, 1);
    text::StringBuilder stringBuilder(4096);
    toString(stringBuilder, level);
    return stringBuilder.toString();
}

void Value::toString(text::StringBuilder &stringBuilder, size_t level) const noexcept {
    level -= 1;
    switch (getType()) {
        case Type::Null:
            stringBuilder << "<NULL>";
            break;
        case Type::Bool:
            stringBuilder << (std::get<bool>(data) ? "<TRUE>" : "<FALSE>");
            break;
        case Type::Int:
            stringBuilder << std::to_string(std::get<int>(data));
            break;
        case Type::Double:
            stringBuilder << std::to_string(std::get<double>(data));
            break;
        case Type::String:
            stringBuilder << '"';
            stringBuilder << std::get<String>(data);
            stringBuilder << '"';
            break;
        case Type::Blob: {
            auto blob = std::get_if<Blob>(&data);
            char hexBuf[16]{};
            snprintf(hexBuf, 16, "0x%p", blob->data());
            stringBuilder << "<p:";
            stringBuilder.append(hexBuf, 15);
            stringBuilder << " l:";
            stringBuilder << std::to_string(blob->size());
            stringBuilder << ">";
            break;
        }
        case Type::List: {
            auto list = std::get_if<List>(&data);
            auto first = true;
            stringBuilder << '[';
            if (level) {
                for (auto &&item: *list) {
                    if (first) {
                        first = false;
                    } else {
                        stringBuilder << ',';
                    }
                    item.toString(stringBuilder, level);
                }
            } else {
                stringBuilder << "...";
            }
            stringBuilder << ']';
            break;
        }
        case Type::Dict: {
            auto list = std::get_if<Dict>(&data);
            auto first = true;
            stringBuilder << '{';
            if (level) {
                for (auto &&item: *list) {
                    if (first) {
                        first = false;
                    } else {
                        stringBuilder << ',';
                    }
                    stringBuilder << '"';
                    stringBuilder << item.first;
                    stringBuilder << "\":";
                    item.second->toString(stringBuilder, level);
                }
            } else {
                stringBuilder << "...";
            }
            stringBuilder << '}';
            break;
        }
    }
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

List::Iterator List::erase(Iterator it) { return vector.erase(it); }

List::ConstIterator List::erase(ConstIterator it) { return vector.erase(it); }

List::Iterator List::erase(Iterator first, Iterator last) {
    return vector.erase(first, last);
}

List::ConstIterator List::erase(ConstIterator first, ConstIterator last) {
    return vector.erase(first, last);
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

void List::append(Value &&value) & { vector.emplace_back(std::move(value)); }

void List::append(bool value) & { vector.emplace_back(value); }

void List::append(int value) & { vector.emplace_back(value); }

void List::append(double value) & { vector.emplace_back(value); }

void List::append(const char *value) & { vector.emplace_back(value); }

void List::append(String &&value) & { vector.emplace_back(std::move(value)); }

void List::append(const char *bytes, size_t length) & { vector.emplace_back(bytes, length); }

void List::append(Blob &&value) & { vector.emplace_back(std::move(value)); }

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

List &&List::append(String &&value) && {
    vector.emplace_back(std::move(value));
    return std::move(*this);
}

List &&List::append(Blob &&value) && {
    vector.emplace_back(std::move(value));
    return std::move(*this);
}

List &&List::append(const char *bytes, size_t length) && {
    vector.emplace_back(bytes, length);
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
    return vector.insert(it, std::move(value));
}

Dict::~Dict() {
    clear();
}

Dict::Dict(sese::Value::Dict &&other) noexcept {
    std::swap(this->map, other.map);
}

Dict &Dict::operator=(sese::Value::Dict &&other) noexcept {
    std::swap(this->map, other.map);
    return *this;
}

bool Dict::empty() const {
    return map.empty();
}

size_t Dict::size() const {
    return map.size();
}

void Dict::clear() {
    for (auto &it: map) {
        delete it.second;
    }
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
    delete it->second;
    return map.erase(it);
}

Dict::Iterator Dict::erase(Dict::ConstIterator it) {
    delete it->second;
    return map.erase(it);
}

Value *Dict::find(const Value::String &key) {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second;
}

const Value *Dict::find(const Value::String &key) const {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second;
}

void Dict::set(const Value::String &key, Value &&value) & {
    map[key] = new Value(std::move(value));
}

void Dict::set(const Value::String &key, bool value) & {
    map[key] = new Value(value);
}

void Dict::set(const Value::String &key, int value) & {
    map[key] = new Value(value);
}

void Dict::set(const Value::String &key, double value) & {
    map[key] = new Value(value);
}

void Dict::set(const Value::String &key, const char *value) & {
    map[key] = new Value(value);
}

void Dict::set(const Value::String &key, String &&value) & {
    map[key] = new Value(std::move(value));
}

void Dict::set(const Value::String &key, Blob &&value) & {
    map[key] = new Value(std::move(value));
}

void Dict::set(const Value::String &key, const char *value, size_t length) & {
    map[key] = new Value(value, length);
}

void Dict::set(const Value::String &key, List &&value) & {
    map[key] = new Value(std::move(value));
}

void Dict::set(const Value::String &key, Dict &&value) & {
    map[key] = new Value(std::move(value));
}

Dict &&Dict::set(const Value::String &key, Value &&value) && {
    map[key] = new Value(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, bool value) && {
    map[key] = new Value(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, int value) && {
    map[key] = new Value(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, double value) && {
    map[key] = new Value(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value) && {
    map[key] = new Value(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, String &&value) && {
    map[key] = new Value(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Blob &&value) && {
    map[key] = new Value(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value, size_t length) && {
    map[key] = new Value(value, length);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, List &&value) && {
    map[key] = new Value(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Dict &&value) && {
    map[key] = new Value(std::move(value));
    return std::move(*this);
}

bool Dict::remove(const Value::String &key) {
    return map.erase(key) > 0;
}