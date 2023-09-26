/// \file SStringBuilder.h
/// \date 2022-9-27
/// \version 0.1
/// \author kaoru
/// \brief 包含 SStringBuilder

#pragma once
#include <sese/text/SString.h>

namespace sstr {

    /// SString 构造器
    class API SStringBuilder final {
        // 构造相关
    public:
        SStringBuilder(const SStringBuilder &builder);
        SStringBuilder(SStringBuilder &&builder) noexcept;
        explicit SStringBuilder(size_t bufferSize);
        ~SStringBuilder();

        // 基础功能
    public:
        const uint32_t *data() const;
        size_t size() const;
        size_t cap() const;

        bool null() const;
        bool emtpy() const;
        /// 扩容
        /// \param size 扩容大小，单位为 4 bytes
        /// \return 操作是否成功
        bool reserve(size_t size);
        void trim();
        void reverse();
        int32_t find(const char *str) const;
        int32_t find(const SStringView &str) const;
        void append(const char *str);
        void append(const SStringView &str);
        
        // 不会支持
        // std::vector<SString> split(const char *str) const;
        // std::vector<SString> split(const SString &str) const;

        void clear();

        SChar at(size_t index) const;
        void set(size_t index, SChar ch);
        void remove(size_t index);
        void remove(size_t begin, size_t len);
        void substring(size_t begin);
        void substring(size_t begin, size_t len);
        void insert(size_t index, SChar ch);
        void insert(size_t index, const char *str);
        void insert(size_t index, const SStringView &str);
        void replace(size_t begin, size_t len, const char *str);
        void replace(size_t begin, size_t len, const SStringView &str);
    
        SString toString() const;

    private:
        /// 数据指针
        uint32_t *_data = nullptr;
        /// 字符个数
        size_t _size = 0;
        /// 容量（单位 uint32_t 即 4 bytes）
        size_t _cap = 0;
    };

}// namespace sstr