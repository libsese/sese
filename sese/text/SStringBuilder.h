/// \file SStringBuilder.h
/// \date 2022-9-27
/// \version 0.1
/// \author kaoru
/// \brief 包含 SStringBuilder

#pragma once

#include <sese/Config.h>
#include <sese/text/SString.h>

namespace sstr {

/// SString 构造器
class  SESE_DEPRECATED_WITH("please use sese::text::StringBuilder") SStringBuilder final {
    // 构造相关
public:
    SStringBuilder(const SStringBuilder &builder);     // NOLINT
    SStringBuilder(SStringBuilder &&builder) noexcept; // NOLINT
    explicit SStringBuilder(size_t buffer_size);
    ~SStringBuilder();

    // 基础功能
public:
    [[nodiscard]] const uint32_t *data() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t cap() const;

    [[nodiscard]] bool null() const;
    [[nodiscard]] bool emtpy() const;

    bool reserve(size_t size);
    void trim();
    void reverse();
    int32_t find(const char *str) const;
    [[nodiscard]] int32_t find(const SStringView &str) const;
    void append(const char *str);
    void append(const SStringView &str);

    // 不会支持
    // std::vector<SString> split(const char *str) const;
    // std::vector<SString> split(const SString &str) const;

    void clear();

    [[nodiscard]] SChar at(size_t index) const;
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

    [[nodiscard]] SString toString() const;

private:
    /// 数据指针
    uint32_t *_data = nullptr;
    /// 字符个数
    size_t _size = 0;
    /// 容量（单位 uint32_t 即 4 bytes）
    size_t _cap = 0;
};

} // namespace sstr