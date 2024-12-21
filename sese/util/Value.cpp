// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>
#define SESE_C_LIKE_FORMAT

#include <sese/util/Value.h>
#include <sese/text/Number.h>
#include <sese/record/Marco.h>

#include <algorithm>
#include <cassert>
#include <utility>

using sese::Value;
using List = sese::Value::List;
using Dict = sese::Value::Dict;

Value::Value(Type type) {
    switch (type) {
        case Type::NONE:
            data.emplace<Null>();
            break;
        case Type::BOOL:
            data.emplace<bool>(false);
            break;
        case Type::INT:
            data.emplace<Integer>(0);
            break;
        case Type::DOUBLE:
            data.emplace<double>(0.0);
            break;
        case Type::STRING:
            data.emplace<String>();
            break;
        case Type::LIST:
            data.emplace<List>();
            break;
        case Type::DICT:
            data.emplace<Dict>();
            break;
        case Type::BLOB:
            data.emplace<Blob>();
            break;
    }
}

Value::Value(bool value) : data(value) {}

Value::Value(Integer value) : data(value) {}

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
    return Value(Type::LIST);
}

Value Value::dict() {
    return Value(Type::DICT);
}

Value::Type Value::getType() const {
    return static_cast<Type>(data.index());
}

bool Value::isNull() const {
    return getType() == Type::NONE;
}

bool Value::isBool() const {
    return getType() == Type::BOOL;
}

bool Value::isInt() const {
    return getType() == Type::INT;
}

bool Value::isDouble() const {
    return getType() == Type::DOUBLE;
}

bool Value::isString() const {
    return getType() == Type::STRING;
}

bool Value::isBlob() const {
    return getType() == Type::BLOB;
}

bool Value::isList() const {
    return getType() == Type::LIST;
}

bool Value::isDict() const {
    return getType() == Type::DICT;
}

std::optional<bool> Value::getIfBool() const {
    if (isBool()) {
        return std::get<bool>(data);
    } else if (isInt()) {
        return std::get<Integer>(data) != 0;
    } else if (isDouble()) {
        return std::get<double>(data) != 0.0;
    }
    return std::nullopt;
}

std::optional<Value::Integer> Value::getIfInt() const {
    if (isInt()) {
        return std::get<Integer>(data);
    } else if (isDouble()) {
        return static_cast<Integer>(std::get<double>(data));
    }
    return std::nullopt;
}

std::optional<double> Value::getIfDouble() const {
    if (isInt()) {
        return static_cast<double>(std::get<Integer>(data));
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

Value::Integer Value::getInt() const {
    assert(isInt());
    return std::get<Integer>(data);
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
            case Type::NONE:
                return true;
            case Type::BOOL:
                return std::get<bool>(data) == std::get<bool>(rhs.data);
            case Type::INT:
                return std::get<Integer>(data) == std::get<Integer>(rhs.data);
            case Type::DOUBLE:
                return std::get<double>(data) == std::get<double>(rhs.data);
            case Type::STRING:
                return std::get<String>(data) == std::get<String>(rhs.data);
            case Type::BLOB:
                return std::get_if<Blob>(&data) == std::get_if<Blob>(&rhs.data);
            case Type::LIST:
                return std::get_if<List>(&data) == std::get_if<List>(&rhs.data);
            case Type::DICT:
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
    text::StringBuilder string_builder(4096);
    toString(string_builder, level);
    return string_builder.toString();
}

void Value::toString(text::StringBuilder &string_builder, size_t level) const noexcept {
    level -= 1;
    switch (getType()) {
        case Type::NONE:
            string_builder << "<NULL>";
            break;
        case Type::BOOL:
            string_builder << (std::get<bool>(data) ? "<TRUE>" : "<FALSE>");
            break;
        case Type::INT:
            string_builder << std::to_string(std::get<Integer>(data));
            break;
        case Type::DOUBLE:
            string_builder << std::to_string(std::get<double>(data));
            break;
        case Type::STRING:
            string_builder << '"' << std::get<String>(data) << '"';
            break;
        case Type::BLOB: {
            const auto BLOB = std::get_if<Blob>(&data);
            const auto NUMBER = text::Number::toHex(reinterpret_cast<uint64_t>(BLOB->data()));
            string_builder << "<p:0x" << NUMBER << " l:" << std::to_string(BLOB->size()) << ">";
            break;
        }
        case Type::LIST: {
            const auto LIST = std::get_if<List>(&data);
            string_builder << '[';
            if (level) {
                auto first = true;
                for (auto &&item: *LIST) {
                    if (first) {
                        first = false;
                    } else {
                        string_builder << ',';
                    }
                    item->toString(string_builder, level);
                }
            } else {
                string_builder << "...";
            }
            string_builder << ']';
            break;
        }
        case Type::DICT: {
            const auto LIST = std::get_if<Dict>(&data);
            string_builder << '{';
            if (level) {
                auto first = true;
                for (const auto &[fst, snd]: *LIST) {
                    if (first) {
                        first = false;
                    } else {
                        string_builder << ',';
                    }
                    string_builder << '"' << fst << "\":";
                    snd->toString(string_builder, level);
                }
            } else {
                string_builder << "...";
            }
            string_builder << '}';
            break;
        }
    }
}

size_t List::empty() const { return vector.empty(); }

size_t List::size() const { return vector.size(); }

void List::reserve(size_t size) { vector.reserve(size); }

void List::resize(size_t size) { vector.resize(size); }

void List::clear() { vector.clear(); }

const std::shared_ptr<Value> List::operator[](size_t index) const { return vector[index]; }

std::shared_ptr<Value> List::operator[](size_t index) { return vector[index]; }

size_t List::erase(const std::shared_ptr<Value> &value) {
    size_t count = 0;
    vector.erase(
            std::remove_if(
                    vector.begin(),
                    vector.end(),
                    [&value, &count](const std::shared_ptr<Value> &v) {
                        if (*value == *v) {
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

List::Iterator List::erase(Iterator it) {
    return vector.erase(it);
}

List::ConstIterator List::erase(ConstIterator it) {
    return vector.erase(it);
}

List::Iterator List::erase(const Iterator &first, const Iterator &last) {
    return vector.erase(first, last);
}

List::ConstIterator List::erase(const ConstIterator &first, const ConstIterator &last) {
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

const std::shared_ptr<Value> List::front() const {
    assert(!vector.empty());
    return vector.front();
}

std::shared_ptr<Value> List::front() {
    assert(!vector.empty());
    return vector.front();
}

const std::shared_ptr<Value> List::back() const {
    assert(!vector.empty());
    return vector.back();
}

std::shared_ptr<Value> List::back() {
    assert(!vector.empty());
    return vector.back();
}

std::shared_ptr<Value> List::appendRef(Value &&value) {
    auto ptr = std::make_shared<Value>(std::move(value));
    vector.emplace_back(ptr);
    return ptr;
}

void List::append(Value &&value) & { vector.emplace_back(std::make_shared<Value>(std::move(value))); }

void List::append(bool value) & { vector.emplace_back(std::make_shared<Value>(value)); }

void List::append(Integer value) & { vector.emplace_back(std::make_shared<Value>(value)); }

void List::append(double value) & { vector.emplace_back(std::make_shared<Value>(value)); }

void List::append(const char *value) & { vector.emplace_back(std::make_shared<Value>(value)); }

void List::append(String &&value) & { vector.emplace_back(std::make_shared<Value>(std::move(value))); }

void List::append(const char *bytes, size_t length) & { vector.emplace_back(std::make_shared<Value>(bytes, length)); }

void List::append(Blob &&value) & { vector.emplace_back(std::make_shared<Value>(std::move(value))); }

void List::append(List &&value) & { vector.emplace_back(std::make_shared<Value>(std::move(value))); }

void List::append(Dict &&value) & { vector.emplace_back(std::make_shared<Value>(std::move(value))); }

List &&List::append(Value &&value) && {
    vector.emplace_back(std::make_shared<Value>(std::move(value)));
    return std::move(*this);
}

List &&List::append(bool value) && {
    vector.emplace_back(std::make_shared<Value>(value));
    return std::move(*this);
}

List &&List::append(Integer value) && {
    vector.emplace_back(std::make_shared<Value>(value));
    return std::move(*this);
}

List &&List::append(double value) && {
    vector.emplace_back(std::make_shared<Value>(value));
    return std::move(*this);
}

List &&List::append(const char *value) && {
    vector.emplace_back(std::make_shared<Value>(value));
    return std::move(*this);
}

List &&List::append(String &&value) && {
    vector.emplace_back(std::make_shared<Value>(std::move(value)));
    return std::move(*this);
}

List &&List::append(Blob &&value) && {
    vector.emplace_back(std::make_shared<Value>(std::move(value)));
    return std::move(*this);
}

List &&List::append(const char *bytes, size_t length) && {
    vector.emplace_back(std::make_shared<Value>(bytes, length));
    return std::move(*this);
}

List &&List::append(List &&value) && {
    vector.emplace_back(std::make_shared<Value>(std::move(value)));
    return std::move(*this);
}

List &&List::append(Dict &&value) && {
    vector.emplace_back(std::make_shared<Value>(std::move(value)));
    return std::move(*this);
}

List::Iterator List::insert(Iterator it, Value &&value) {
    return vector.insert(it, std::make_shared<Value>(std::move(value)));
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

Dict::Iterator Dict::erase(const Dict::Iterator &it) {
    return map.erase(it);
}

Dict::Iterator Dict::erase(const Dict::ConstIterator &it) {
    return map.erase(it);
}

std::shared_ptr<Value> Dict::find(const Value::String &key) {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second;
}

const std::shared_ptr<Value> Dict::find(const Value::String &key) const {
    auto it = map.find(key);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<Value> Dict::setRef(const String &key, Value &&value) {
    auto [fst, _] = map.emplace(key, std::make_shared<Value>(std::move(value)));
    return fst->second;
}

void Dict::set(const Value::String &key, Value &&value) & {
    map[key] = std::make_shared<Value>(std::move(value));
}

void Dict::set(const Value::String &key, bool value) & {
    map[key] = std::make_shared<Value>(value);
}

void Dict::set(const Value::String &key, Integer value) & {
    map[key] = std::make_shared<Value>(value);
}

void Dict::set(const Value::String &key, double value) & {
    map[key] = std::make_shared<Value>(value);
}

void Dict::set(const Value::String &key, const char *value) & {
    map[key] = std::make_shared<Value>(value);
}

void Dict::set(const Value::String &key, String &&value) & {
    map[key] = std::make_shared<Value>(std::move(value));
}

void Dict::set(const Value::String &key, Blob &&value) & {
    map[key] = std::make_shared<Value>(std::move(value));
}

void Dict::set(const Value::String &key, const char *value, size_t length) & {
    map[key] = std::make_shared<Value>(value, length);
}

void Dict::set(const Value::String &key, List &&value) & {
    map[key] = std::make_shared<Value>(std::move(value));
}

void Dict::set(const Value::String &key, Dict &&value) & {
    map[key] = std::make_shared<Value>(std::move(value));
}

Dict &&Dict::set(const Value::String &key, Value &&value) && {
    map[key] = std::make_shared<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, bool value) && {
    map[key] = std::make_shared<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Integer value) && {
    map[key] = std::make_shared<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, double value) && {
    map[key] = std::make_shared<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value) && {
    map[key] = std::make_shared<Value>(value);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, String &&value) && {
    map[key] = std::make_shared<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Blob &&value) && {
    map[key] = std::make_shared<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, const char *value, size_t length) && {
    map[key] = std::make_shared<Value>(value, length);
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, List &&value) && {
    map[key] = std::make_shared<Value>(std::move(value));
    return std::move(*this);
}

Dict &&Dict::set(const Value::String &key, Dict &&value) && {
    map[key] = std::make_shared<Value>(std::move(value));
    return std::move(*this);
}

bool Dict::remove(const Value::String &key) {
    return map.erase(key) > 0;
}

//std::string sese::text::overload::toString(const Value &t) {
//    return t.toString();
//}
