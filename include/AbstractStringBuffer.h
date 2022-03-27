#pragma once
#include <string>
#include <vector>
#include "Config.h"

namespace sese {

    class AbstractStringBuffer {
    public:
        explicit AbstractStringBuffer(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept;
        explicit AbstractStringBuffer(const char *str) noexcept;
        ~AbstractStringBuffer() noexcept;

    protected:
        size_t cap{};  // 实际容量
        size_t len = 0;// 字符串长度
        char *buffer;  // 字符串缓存

    protected:
        void expansion(size_t newSize) noexcept;

    public:
        void append(char ch);
        void append(const char *str) noexcept;
        void append(const std::string &str) noexcept { this->append(str.c_str()); }
        [[nodiscard]] size_t length() const noexcept { return this->len; }
        [[nodiscard]] size_t size() const noexcept { return this->cap; }
        [[nodiscard]] bool empty() const noexcept { return 0 == this->len; };
        void clear() noexcept;
        void reverse() noexcept;
        [[nodiscard]] char getCharAt(int index) const;
        void setChatAt(int index, char ch);
        void delCharAt(int index);
        void del(int start, int end);
        void insertAt(int index, const char *str);
        void insertAt(int index, const std::string &str) { this->insertAt(index, str.c_str()); };
        void trim() noexcept;
        [[nodiscard]] std::vector<std::string> split(const std::string &str) const noexcept;
        std::string toString();
    };

}// namespace sese