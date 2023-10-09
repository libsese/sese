/**
 * @file StringBuffer.h
 * @brief 线程安全的字符串缓存类
 * @author kaoru
 * @date 2022年4月5日
 */
#pragma once

#include "sese/text/AbstractStringBuffer.h"
#include "sese/Config.h"

#include <mutex>

#ifdef _WIN32
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

namespace sese::text {

/**
 * @brief 线程安全的字符串缓存类
 */
class API StringBuffer final : private AbstractStringBuffer {
public:
    using Ptr = std::unique_ptr<StringBuffer>;

    explicit StringBuffer(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept;
    explicit StringBuffer(const char *str) noexcept;

public:
    void append(char ch) noexcept override;
    void append(const char *str) noexcept override;
    void append(const std::string &str) noexcept override;
    void append(const std::string_view &str) noexcept override;
    void append(const String &str) noexcept override;
    void append(const StringView &str) noexcept override;
    [[nodiscard]] size_t length() noexcept;
    [[nodiscard]] size_t size() noexcept;
    [[nodiscard]] bool empty() noexcept;
    void clear() noexcept override;
    void reverse() noexcept override;
    [[nodiscard]] char getCharAt(int index);
    bool setChatAt(int index, char ch) override;
    bool delCharAt(int index) override;
    bool del(int start, int len) override;
    bool insertAt(int index, const char *str) override;
    bool insertAt(int index, const std::string &str) override;
    bool insertAt(int index, const std::string_view &str) override;
    bool insertAt(int index, const String &str) override;
    bool insertAt(int index, const StringView &str) override;
    void trim() noexcept override;
    [[nodiscard]] std::vector<std::string> split(const std::string &str) noexcept;
    std::string toString() override;
    String toSString() override;

public:
    void operator<<(char ch) noexcept { this->append(ch); }
    void operator<<(const char *str) noexcept { this->append(str); }
    void operator<<(const std::string &str) noexcept { this->append(str); }
    void operator<<(const std::string_view &str) noexcept { this->append(str); }
    void operator<<(const String &str) noexcept { this->append(str); }
    void operator<<(const StringView &str) noexcept { this->append(str); }

private:
    using AbstractStringBuffer::empty;
    using AbstractStringBuffer::getCharAt;
    using AbstractStringBuffer::length;
    using AbstractStringBuffer::size;
    using AbstractStringBuffer::split;

    std::mutex mutex;
};
} // namespace sese::text