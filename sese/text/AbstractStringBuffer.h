/**
 * @file AbstractStringBuffer.h
 * @brief 字符串缓冲类
 * @author kaoru
 * @date 2022年3月28日
 * @version 0.2
 */
#pragma once

#include "sese/Config.h"
#include "sese/text/String.h"

#include <vector>

namespace sese::text {

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

    AbstractStringBuffer(const AbstractStringBuffer &abstract_string_buffer) noexcept;
    AbstractStringBuffer(AbstractStringBuffer &&abstract_string_buffer) noexcept;

    static std::vector<std::string> split(const std::string_view &text, const std::string_view &sub) noexcept;
    static bool startsWith(const std::string_view &text, const std::string_view &prefix) noexcept;
    static bool endsWith(const std::string_view &text, const std::string_view &suffix) noexcept;

protected:
    size_t cap{};           /// 实际容量
    size_t len = 0;         /// 字符串长度
    char *buffer = nullptr; /// 字符串缓存

protected:
    /**
     * 扩容
     * @param new_size 扩容后大小
     */
    void expansion(size_t new_size) noexcept;


    bool insertAt(int index, const char *data, size_t len);

public:
    virtual void append(char ch) noexcept;
    virtual void append(const char *str) noexcept;
    virtual void append(const std::string &str) noexcept;
    virtual void append(const std::string_view &str) noexcept;
    virtual void append(const String &str) noexcept;
    virtual void append(const StringView &view) noexcept;
    virtual void append(const char *data, size_t len) noexcept;
    [[nodiscard]] virtual size_t length() const noexcept { return this->len; }
    [[nodiscard]] virtual size_t size() const noexcept { return this->cap; }
    [[nodiscard]] virtual bool empty() const noexcept { return 0 == this->len; };
    virtual void clear() noexcept;
    virtual void reverse() noexcept;
    [[nodiscard]] virtual char getCharAt(int index) const;
    virtual bool setChatAt(int index, char ch);
    virtual bool delCharAt(int index);
    virtual bool del(int start, int l);
    virtual bool insertAt(int index, const char *str);
    virtual bool insertAt(int index, const std::string &str);
    virtual bool insertAt(int index, const std::string_view &str);
    virtual bool insertAt(int index, const String &str);
    virtual bool insertAt(int index, const StringView &view);
    /// 去除两端空格
    virtual void trim() noexcept;
    [[nodiscard]] virtual std::vector<std::string> split(const std::string_view &str) const noexcept;
    [[nodiscard]] virtual bool startsWith(const std::string_view &prefix) const noexcept;
    [[nodiscard]] virtual bool endsWith(const std::string_view &suffix) const noexcept;
    virtual std::string toString();
    virtual String toSString();
    virtual const void *buf();
};

} // namespace sese::text