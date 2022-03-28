/**
 * @file AbstractStringBuffer.h
 * @brief 字符串缓冲类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include <string>
#include <vector>
#include "Config.h"

namespace sese {

    /**
     * @brief 字符串缓冲类
     */
    class AbstractStringBuffer {
    public:
        /**
         * @param cap 初始容量
         */
        explicit AbstractStringBuffer(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept;
        /**
         * @brief 以字符串为初始化参数
         * @param str 字符串
         */
        explicit AbstractStringBuffer(const char *str) noexcept;
        ~AbstractStringBuffer() noexcept;

    protected:
        size_t cap{};  /// 实际容量
        size_t len = 0;/// 字符串长度
        char *buffer;  /// 字符串缓存

    protected:
        /**
         * 扩容
         * @param newSize 扩容后大小
         */
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
        /// 去除两端空格
        void trim() noexcept;
        [[nodiscard]] std::vector<std::string> split(const std::string &str) const noexcept;
        std::string toString();
    };

}// namespace sese