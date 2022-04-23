#include "sese/AbstractStringBuffer.h"
#include "sese/IndexOutOfBoundsException.h"
#include "sese/Util.h"
#include <cstring>

#ifdef _WIN32
#pragma warning(disable : 4018)
#pragma warning(disable : 4273)
#endif

namespace sese {

    AbstractStringBuffer::AbstractStringBuffer(size_t cap) noexcept {
        this->cap = cap;
        this->buffer = new char[cap];
    }

    AbstractStringBuffer::AbstractStringBuffer(const char *str) noexcept {
        this->len = strlen(str);
        this->cap = (len / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
        this->buffer = new char[cap];
        memcpy(this->buffer, str, len);
    }

    AbstractStringBuffer::~AbstractStringBuffer() noexcept {
        if (this->buffer != nullptr) {
            delete[] this->buffer;
        }
    }

    AbstractStringBuffer::AbstractStringBuffer(AbstractStringBuffer &abstractStringBuffer) noexcept {
        this->cap = abstractStringBuffer.cap;
        this->len = abstractStringBuffer.len;
        this->buffer = new char[this->cap];
        memcpy(this->buffer, abstractStringBuffer.buffer, this->len);
    }

    AbstractStringBuffer::AbstractStringBuffer(AbstractStringBuffer &&abstractStringBuffer) noexcept {
        this->cap = abstractStringBuffer.cap;
        this->len = abstractStringBuffer.len;
        this->buffer = abstractStringBuffer.buffer;

        abstractStringBuffer.cap = 0;
        abstractStringBuffer.len = 0;
        abstractStringBuffer.buffer = nullptr;
    }

    void AbstractStringBuffer::append(const char *str) noexcept {
        auto l = strlen(str);
        if (l > cap - this->len) {
            // 触发扩容
            auto newSize = ((l + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
            this->expansion(newSize);
        }
        memcpy(&this->buffer[this->len], str, l);
        this->len += l;
    }

    void AbstractStringBuffer::expansion(size_t newSize) noexcept {
        char *newBuffer = new char[newSize];
        memcpy(newBuffer, this->buffer, len);
        delete[] this->buffer;
        this->buffer = newBuffer;
        this->cap = newSize;
    }

    std::string AbstractStringBuffer::toString() {
        //        std::shared_ptr<char> str(new char[this->len + 1], [](const char *p) { delete[] p; });
        //        memcpy(str.get(), this->buffer, this->len + 1);
        //        str.get()[this->len] = '\0';
        //        return {str.get()};
        if (1 > cap - this->len) {
            // 触发扩容
            auto newSize = ((1 + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
            this->expansion(newSize);
        }
        this->buffer[this->len] = '\0';
        return {this->buffer};
    }

    void AbstractStringBuffer::clear() noexcept {
        memset(this->buffer, 0, cap);
        this->len = 0;
    }

    std::vector<std::string> AbstractStringBuffer::split(const std::string &str) const noexcept {
        std::vector<std::string> v;
        auto s = std::string(this->buffer);
        std::string::size_type pos1, pos2;
        pos2 = s.find(str);
        pos1 = 0;
        while (std::string::npos != pos2) {
            v.push_back(s.substr(pos1, pos2 - pos1));

            pos1 = pos2 + str.size();
            pos2 = s.find(str, pos1);
        }
        if (pos1 != s.length()) {
            v.push_back(s.substr(pos1));
        }
        return v;
    }

    char AbstractStringBuffer::getCharAt(int index) const {
        // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
        if (!CheckRange(index, this->cap)) throw IndexOutOfBoundsException();
        return this->buffer[index];
    }

    void AbstractStringBuffer::setChatAt(int index, char ch) {
        // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
        if (!CheckRange(index, this->cap)) throw IndexOutOfBoundsException();
        this->buffer[index] = ch;
    }

    void AbstractStringBuffer::reverse() noexcept {
        char temp = '\0';
        for (int j = 0; j < len / 2; j++) {
            temp = this->buffer[j];
            this->buffer[j] = this->buffer[len - 1 - j];
            this->buffer[len - 1 - j] = temp;
        }
    }

    void AbstractStringBuffer::delCharAt(int index) {
        // if (this->cap <= index || index < 0) throw IndexOutOfBoundsException();
        if (!CheckRange(index, this->cap)) throw IndexOutOfBoundsException();
        for (int i = index; i < len - 1; i++) {
            this->buffer[i] = this->buffer[i + 1];
        }
        this->buffer[len - 1] = '\0';
        this->len -= 1;
    }

    void AbstractStringBuffer::del(int start, int end) {
        if (0 < start) throw IndexOutOfBoundsException();
        if (this->cap < end) throw IndexOutOfBoundsException();
        int delCount = end - start + 1;
        for (int i = start; i < len - delCount; i++) {
            if (i + delCount > len) {
                this->buffer[i] = '\0';
            } else {
                this->buffer[i] = this->buffer[i + delCount];
                this->buffer[i + delCount] = '\0';
            }
        }
        this->len -= delCount;
    }

    void AbstractStringBuffer::insertAt(int index, const char *str) {
        if (this->cap <= index) throw IndexOutOfBoundsException();
        size_t l = strlen(str);
        if (l > cap - this->len) {
            // 触发扩容
            auto newSize = ((l + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
            this->expansion(newSize);
        }

        for (size_t i = 0; i < len - index + 1; i++) {
            this->buffer[len + l - i] = this->buffer[len - i];
        }
        memcpy(&this->buffer[index], str, l);
    }

    void AbstractStringBuffer::trim() noexcept {
        size_t w = 0;
        for (size_t i = 0; i < len; i++) {
            if (isSpace(this->buffer[len - 1 - i])) {
                this->buffer[len - 1 - i] = '\0';
            } else {
                w = i;
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
        if (1 > cap - this->len) {
            // 触发扩容
            auto newSize = ((1 + this->len) / STRING_BUFFER_SIZE_FACTOR + 1) * STRING_BUFFER_SIZE_FACTOR;
            this->expansion(newSize);
        }
        this->buffer[this->len] = ch;
        this->len += 1;
    }

}// namespace sese