/// \file SString.h
/// \date 2022-9-27
/// \version 0.1
/// \author kaoru
/// \brief 包含 SString 核心功能， SChar、SStringView 和 SString

#pragma once

#include <sese/Config.h>

#include <vector>

namespace sstr {

    /// Unicode 字符
    struct API SChar final {
        uint32_t code = 0;

        explicit SChar(uint32_t code) noexcept;

        bool operator==(const SChar &ch) const;
        bool operator<(const SChar &ch) const;

        bool operator<=(const SChar &ch) const;
        bool operator>(const SChar &ch) const;
        bool operator>=(const SChar &ch) const;
        bool operator!=(const SChar &ch) const;

        SChar operator+(const SChar &ch) const;
        SChar operator-(const SChar &ch) const;

        explicit operator uint32_t() const;
    };

    /// 获取 UTF-8 字符代码
    /// \param u8char UTF-8 字符
    /// \return 字符代码
    API SChar getUnicodeFromUTF8Char(const char *u8char);

    /// 获取 UTF-8 字符串长度
    /// \param str 字符串
    /// \return 字符串长度
    API size_t getStringLengthFromUTF8String(const char *str);

    /// 获取 UTF-8 字符串字节长度
    /// \param str 目标字符串
    /// \return 字符串字节长度
    API size_t getByteLengthFromUTF8String(const char *str);

    /// 获取 UTF-8 字符占位字节数
    /// \param ch 目标字符
    /// \return 字符占位字节数
    API char getSizeFromUTF8Char(char ch);

    /// 从 SChar 中获取该字符在 UTF-8 中的字节占位字节数
    /// \param ch Unicode 字符
    /// \return 占位字节数
    API char getUTF8SizeFromUnicodeChar(SChar ch);

    /// 从 UTF-8 字符串中获取 Unicode 字符
    /// \param size 该 UTF-8 占位字节数
    /// \param ch UTF-8 字符起始位置
    /// \return Unicode 字符
    API SChar getUnicodeCharFromUTF8Char(char size, const char *ch);

    class SString;

    /// 字符串视图
    class API SStringView {
    public:
        SStringView() noexcept = default;
        explicit SStringView(const char *u8str) noexcept;
        virtual ~SStringView() = default;

    public:
        /// Unicode 字符迭代器
        class Iterator {
        public:
            using difference_type = SChar;
            using value_type = SChar;
            using pointer = const SChar *;
            using reference = const SChar &;
            using iterator_category = std::forward_iterator_tag;

            Iterator(const char *ref, size_t size, size_t pos = 0);

            Iterator operator++();
            Iterator operator++(int c);

            bool operator==(const Iterator &other) const;
            bool operator!=(const Iterator &other) const;
            SChar operator*();

        private:
            Iterator() = default;

            const char *_ref = nullptr;
            size_t _pos = 0;
            size_t _size = 0;
            SChar _ch = SChar(0);
        };

    public:
        Iterator begin();
        Iterator end();

        /// data 是否为 nullptr
        /// \retval true
        /// \retval false
        [[nodiscard]] bool null() const;

        /// 字符串是否为空
        /// \retval true 字符串为空
        /// \retval false 字符串不为空
        [[nodiscard]] bool empty() const;

        /// 获取字符串中字符个数
        /// \return 字符个数
        [[nodiscard]] size_t len() const;

        /// 获取字符串字节数
        /// \return 字符串字节数
        [[nodiscard]] virtual size_t size() const;

        /// 获取缓冲区指针
        /// \return 缓冲区指针
        [[nodiscard]] const char *data() const;

        /// 查找字符串，索引单位是字数
        /// \param str 子串
        /// \return 子串位置
        [[nodiscard]] int32_t find(const SStringView &str) const;

        /// 查找字符串，索引单位是字数
        /// \param u8str 子串
        /// \return 子串位置
        int32_t find(const char *u8str) const;

        /// 查找字节串，索引单位是字节
        /// \param bytes 子串
        /// \return 子串位置
        int32_t findByBytes(const char *bytes) const;

        /// 除去字符串两端空格
        /// \note 注意是空格
        /// \return 处理后对象
        [[nodiscard]] SString trim() const;

        /// 反转字符串
        /// \return 反转后对象
        [[nodiscard]] SString reverse() const;

        /// 尾加字符串
        /// \param str 待尾加字符串
        /// \return 尾加结果字符串
        [[nodiscard]] SString append(const SStringView &str) const;

        /// 尾加字符串
        /// \param u8str 待尾加字符串
        /// \return 尾加结果字符串
        SString append(const char *u8str) const;

        /// 切割字符串
        /// \param str 切割标识符
        /// \return 切割结果
        [[nodiscard]] std::vector<SString> split(const SStringView &str) const;

        /// 切割字符串
        /// \deprecated 尾加对象的字符串编码必须也是 UTF-8，否则不建议使用
        /// \param str 切割标识符
        /// \return 切割结果
        std::vector<SString> split(const char *str) const;

        [[nodiscard]] SString substring(size_t begin) const;
        /// \brief 截取子串 [begin, begin + len - 1]
        /// \param begin 起始字符索引
        /// \param len 截取长度
        /// \return 子串
        [[nodiscard]] SString substring(size_t begin, size_t len) const;

        /// \brief 字符串是否以某个子串结尾
        /// \param str 匹配子串
        /// \return 结果
        [[nodiscard]] bool endsWith(const SStringView &str) const;

        /// 字母是否为全小写
        [[nodiscard]] bool isLower() const;
        /// 字母是否为全大写
        [[nodiscard]] bool isUpper() const;

        /// 创建字母转为全小写的副本
        [[nodiscard]] SString toLower() const;
        /// 创建字母转为全大写的副本
        [[nodiscard]] SString toUpper() const;

        [[nodiscard]] SChar at(size_t index) const;
        [[nodiscard]] std::vector<SChar> toChars() const;
        [[nodiscard]] std::string toString() const;
        [[nodiscard]] std::wstring toWString() const;
        [[nodiscard]] std::unique_ptr<wchar_t[]> toCWString() const;

    public:
        SChar operator[](size_t index) const;
        bool operator!=(const SStringView &str) const;
        bool operator!=(const char *u8str) const;
        bool operator==(const SStringView &str) const;
        bool operator==(const char *u8str) const;
        SString operator+(const SStringView &str) const;
        SString operator+(const char *u8str) const;

    protected:
        char *_data = nullptr;
        size_t _size = 0;
    };

    /// 字符串
    class API SString final : public SStringView {
    public:
        friend class SStringView;

        explicit SString() noexcept;
        SString(const char *str, size_t size);
        SString(const SString &sString) noexcept;
        SString(SString &&sString) noexcept;
        ~SString() noexcept override;

        static SString fromSChars(SChar ch[], size_t size);
        static SString fromSChars(std::vector<SChar> &chars);
        static SString fromUTF8(const char *str);
        static SString fromUCS2LE(const wchar_t *str);

    public:
        /// 获取缓存区容量
        /// \return 缓冲区容量
        [[nodiscard]] size_t cap() const;
        /// 获取缓冲区已用大小
        /// \return 缓冲区已用大小
        [[nodiscard]] size_t size() const override;

        /// 将字符串转换为全小写的形式
        void toLower();
        /// 将字符串转换为全大写的形式
        void toUpper();

        /// \brief 获取 data 指针
        /// \deprecated 通常不应该使用该函数
        char *data();

    public:
        void operator+=(const SStringView &str);
        void operator+=(const char *u8str);

    protected:
        size_t _capacity = 0;
    };
}// namespace sstr