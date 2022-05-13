/**
 * @file AbstractStringBuffer.h
 * @brief 字符串缓冲类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "sese/Config.h"
#include <vector>

namespace sese {

    /**
     * @brief 字符串缓冲类
     */
    class API AbstractStringBuffer {
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
        virtual ~AbstractStringBuffer() noexcept;

        AbstractStringBuffer(AbstractStringBuffer &abstractStringBuffer) noexcept;
        AbstractStringBuffer(AbstractStringBuffer &&abstractStringBuffer) noexcept;

    protected:
        size_t cap{};           /// 实际容量
        size_t len = 0;         /// 字符串长度
        char *buffer = nullptr; /// 字符串缓存

    protected:
        /**
         * 扩容
         * @param newSize 扩容后大小
         */
        void expansion(size_t newSize) noexcept;

    public:
        virtual void append(char ch) noexcept;
        virtual void append(const char *str) noexcept;
        void append(const std::string &str) noexcept { this->append(str.c_str()); }
        [[nodiscard]] virtual size_t length() const noexcept { return this->len; }
        [[nodiscard]] virtual size_t size() const noexcept { return this->cap; }
        [[nodiscard]] virtual bool empty() const noexcept { return 0 == this->len; };
        virtual void clear() noexcept;
        virtual void reverse() noexcept;
        [[nodiscard]] virtual char getCharAt(int index) const;
        virtual bool setChatAt(int index, char ch);
        virtual bool delCharAt(int index);
        virtual bool del(int start, int end);
        virtual bool insertAt(int index, const char *str);
        virtual void insertAt(int index, const std::string &str) { this->insertAt(index, str.c_str()); };
        /// 去除两端空格
        virtual void trim() noexcept;
        [[nodiscard]] virtual std::vector<std::string> split(const std::string &str) const noexcept;
        virtual std::string toString();
    };

}// namespace sese