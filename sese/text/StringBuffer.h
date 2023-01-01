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
    class API StringBuffer : public AbstractStringBuffer {
    public:
        using Ptr = std::unique_ptr<StringBuffer>;

        explicit StringBuffer(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept;
        explicit StringBuffer(const char *str) noexcept;

    public:
        void append(char ch) noexcept override;
        void append(const char *str) noexcept override;
        [[nodiscard]] size_t length() noexcept;
        [[nodiscard]] size_t size() noexcept;
        [[nodiscard]] bool empty() noexcept;
        void clear() noexcept override;
        void reverse() noexcept override;
        [[nodiscard]] char getCharAt(int index)  ;
        bool setChatAt(int index, char ch) override;
        bool delCharAt(int index) override;
        bool del(int start, int end) override;
        bool insertAt(int index, const char *str) override;
        void insertAt(int index, const std::string &str) override;
        void trim() noexcept override;
        [[nodiscard]] std::vector<std::string> split(const std::string &str) noexcept;
        std::string toString() override;

    private:
        std::mutex mutex;
    };
}// namespace sese