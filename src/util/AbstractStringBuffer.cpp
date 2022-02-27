#include "AbstractStringBuffer.h"
#include "misc.h"
#include <cstring>

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
        return std::string(this->buffer);
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

    [[maybe_unused]] char AbstractStringBuffer::getCharAt(int index) const {
        return this->buffer[index];
    }

    void AbstractStringBuffer::setChatAt(int index, char ch) {
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
        for (int i = index; i < len - 1; i++) {
            this->buffer[i] = this->buffer[i + 1];
        }
        this->buffer[len - 1] = '\0';
        this->len -= 1;
    }

    void AbstractStringBuffer::del(int start, int end) {
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
        return;
    }

}// namespace sese