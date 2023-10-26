#include "sese/text/AbstractStringBuffer.h"
#include "sese/util/Util.h"

#include <cstring>

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4273)
#endif

namespace sese::text {

AbstractStringBuffer::AbstractStringBuffer(size_t cap) noexcept {
    this->cap = cap;
    this->buffer = new char[cap];
    memset(this->buffer, 0, cap);
}

AbstractStringBuffer::AbstractStringBuffer(const char *str) noexcept {
    this->len = strlen(str);
    this->cap = (len / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
    this->buffer = new char[cap];
    memcpy(this->buffer, str, len);
    memset(this->buffer + len, 0, this->cap - len);
}

AbstractStringBuffer::~AbstractStringBuffer() noexcept {
    if (this->buffer != nullptr) { // NOLINT
        delete[] this->buffer;     // GCOVR_EXCL_LINE
    }
}

AbstractStringBuffer::AbstractStringBuffer(AbstractStringBuffer &abstractStringBuffer) noexcept {
    this->cap = abstractStringBuffer.cap;
    this->len = abstractStringBuffer.len;
    this->buffer = new char[this->cap];
    memcpy(this->buffer, abstractStringBuffer.buffer, this->len);
    memset(this->buffer + this->len, 0, this->cap - this->len);
}

AbstractStringBuffer::AbstractStringBuffer(AbstractStringBuffer &&abstractStringBuffer) noexcept {
    this->cap = abstractStringBuffer.cap;
    this->len = abstractStringBuffer.len;
    this->buffer = abstractStringBuffer.buffer;

    abstractStringBuffer.cap = 0;
    abstractStringBuffer.len = 0;
    abstractStringBuffer.buffer = nullptr;
}

std::vector<std::string> AbstractStringBuffer::split(const std::string &text, const std::string &sub) noexcept {
    std::vector<std::string> v;
    std::string::size_type pos1, pos2;
    pos2 = text.find(sub);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.emplace_back(text.data() + pos1, pos2 - pos1);

        pos1 = pos2 + sub.size();
        pos2 = text.find(sub, pos1);
    }
    if (pos1 != text.length()) {
        v.emplace_back(text.data() + pos1);
    }
    return v;
}

void AbstractStringBuffer::append(const char *data, size_t l) noexcept {
    if (l > cap - this->len) {
        // 触发扩容
        auto newSize = ((l + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
        this->expansion(newSize);
    }
    memcpy(&this->buffer[this->len], data, l);
    this->len += l;
}

void AbstractStringBuffer::append(const char *str) noexcept {
    auto l = strlen(str);
    append(str, l);
}

void AbstractStringBuffer::append(const std::string &str) noexcept {
    append(str.data(), str.length());
}

void AbstractStringBuffer::append(const std::string_view &str) noexcept {
    append(str.data(), str.length());
}

void AbstractStringBuffer::append(const String &str) noexcept {
    append(((StringView *) &str)->data(), str.len());
}

void AbstractStringBuffer::append(const StringView &view) noexcept {
    append(view.data(), view.len());
}

void AbstractStringBuffer::expansion(size_t newSize) noexcept {
    char *newBuffer = new char[newSize];
    memcpy(newBuffer, this->buffer, len);
    delete[] this->buffer; // GCOVR_EXCL_LINE
    this->buffer = newBuffer;
    this->cap = newSize;
}

std::string AbstractStringBuffer::toString() {
    auto view = std::string_view(this->buffer, this->len);
    return {view.begin(), view.end()}; // GCOVR_EXCL_LINE
}

String AbstractStringBuffer::toSString() {
    return {this->buffer, this->len};
}

void AbstractStringBuffer::clear() noexcept {
    // memset(this->buffer, 0, cap);
    this->len = 0;
}

std::vector<std::string> AbstractStringBuffer::split(const std::string &str) const noexcept {
    std::vector<std::string> v;
    //        auto s = std::string(this->buffer);
    auto s = std::string_view(this->buffer, this->len);
    std::string::size_type pos1, pos2;
    pos2 = s.find(str);
    pos1 = 0;
    while (std::string::npos != pos2) {
        //            v.push_back(s.substr(pos1, pos2 - pos1));
        v.emplace_back(s.data() + pos1, pos2 - pos1);

        pos1 = pos2 + str.size();
        pos2 = s.find(str, pos1);
    }
    if (pos1 != s.length()) {
        //            v.push_back(s.substr(pos1));
        v.emplace_back(s.data() + pos1, this->len - pos1);
    }
    return v;
}

char AbstractStringBuffer::getCharAt(int index) const {
    // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
    if (!CheckRange(index, this->cap)) return 0;
    return this->buffer[index];
}

bool AbstractStringBuffer::setChatAt(int index, char ch) {
    // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
    if (!CheckRange(index, this->cap)) return false;
    this->buffer[index] = ch;
    return true;
}

void AbstractStringBuffer::reverse() noexcept {
    char temp = '\0';
    for (int j = 0; j < len / 2; j++) {
        temp = this->buffer[j];
        this->buffer[j] = this->buffer[len - 1 - j];
        this->buffer[len - 1 - j] = temp;
    }
}

bool AbstractStringBuffer::delCharAt(int index) {
    // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
    if (!CheckRange(index, this->len)) return false;
    for (int i = index; i < len - 1; i++) {
        this->buffer[i] = this->buffer[i + 1];
    }
    this->buffer[len - 1] = '\0';
    this->len -= 1;
    return true;
}

bool AbstractStringBuffer::del(int start, int l) {
    if (0 > start) {
        return false;
    }
    if (this->len < start + l) {
        return false;
    }
    memmove(&this->buffer[start], &this->buffer[start + l], this->len - start - l);
    this->len -= l;
    this->buffer[this->len] = '\0';
    return true;
}

bool AbstractStringBuffer::insertAt(int index, const char *data, size_t l) {
    if (this->cap <= index) {
        return false;
    }
    if (l > cap - this->len) {
        // 触发扩容
        auto newSize = ((l + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
        this->expansion(newSize);
    }
    memmove(&this->buffer[index + l], &this->buffer[index], len);
    memcpy(&this->buffer[index], data, l);
    this->len += l;
    return true;
}

bool AbstractStringBuffer::insertAt(int index, const char *str) {
    auto l = strlen(str);
    return insertAt(index, str, l);
}

bool AbstractStringBuffer::insertAt(int index, const std::string &str) {
    return insertAt(index, str.data(), str.length());
}

bool AbstractStringBuffer::insertAt(int index, const std::string_view &str) {
    return insertAt(index, str.data(), str.length());
}

bool AbstractStringBuffer::insertAt(int index, const String &str) {
    return insertAt(index, ((StringView *) &str)->data(), str.len());
}

bool AbstractStringBuffer::insertAt(int index, const StringView &view) {
    return insertAt(index, view.data(), view.len());
}


void AbstractStringBuffer::trim() noexcept {
    size_t w = 0;
    for (size_t i = 0; i < len; i++) {
        if (isSpace(this->buffer[len - 1 - i])) {
            this->buffer[len - 1 - i] = '\0';
            w += 1;
        } else {
            break;
        }
    }
    this->len -= w;

    w = 0;
    for (int i = 0; i < this->len; i++) {
        if (!isSpace(this->buffer[i])) {
            w = i;
            break;
        }
    }
    for (int i = 0; i < len - w; i++) {
        this->buffer[i] = this->buffer[i + w];
    }
    memset(&this->buffer[len - w], 0, w + 1);
    this->len -= w;
}

void AbstractStringBuffer::append(char ch) noexcept {
    append(&ch, 1);
}

} // namespace sese::text