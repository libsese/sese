// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// \file SString.h
/// \date September 27, 2022
/// \version 0.1
/// \author kaoru
/// \brief Contains the core functionalities of SString, SChar, SStringView, and SString

#pragma once

#include <sese/Config.h>

#include <vector>

namespace sstr {

/// Unicode character
struct SChar final {
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

/// Get UTF-8 character code
/// \param u8char UTF-8 character
/// \return Character code
 SChar getUnicodeFromUTF8Char(const char *u8char);

/// Get UTF-8 string length
/// \param str String
/// \return String length
 size_t getStringLengthFromUTF8String(const char *str);

/// Get UTF-8 string byte length
/// \param str Target string
/// \return Byte length of the string
 size_t getByteLengthFromUTF8String(const char *str);

/// Get the number of bytes occupied by a UTF-8 character
/// \param ch Target character
/// \return Number of bytes occupied by the character
 char getSizeFromUTF8Char(char ch);

/// Get the number of bytes a character occupies in UTF-8 from SChar
/// \param ch Unicode character
/// \return Number of bytes occupied
 char getUTF8SizeFromUnicodeChar(SChar ch);

/// Get Unicode character from UTF-8 string
/// \param size Number of bytes this UTF-8 character occupies
/// \param ch Starting position of the UTF-8 character
/// \return Unicode character
 SChar getUnicodeCharFromUTF8Char(char size, const char *ch);

class SString;

/// String view
class  SStringView {
public:
    SStringView() noexcept = default;
    explicit SStringView(const char *u8str) noexcept;
    virtual ~SStringView() = default;

public:
    /// Unicode character iterator
    class Iterator {
    public:
        using difference_type = SChar;
        using value_type = SChar;
        using pointer = const SChar *;
        using reference = const SChar &;
        using iterator_category = std::forward_iterator_tag;

        Iterator(const char *ref, size_t size, size_t pos = 0);

        Iterator operator++();
        // Iterator operator++(int c);

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

    /// Check if data is nullptr
    /// \retval true
    /// \retval false
    [[nodiscard]] bool null() const;

    /// Check if the string is empty
    /// \retval true String is empty
    /// \retval false String is not empty
    [[nodiscard]] bool empty() const;

    /// Get the number of characters in the string
    /// \return Number of characters
    [[nodiscard]] size_t len() const;

    /// Get the number of bytes in the string
    /// \return Number of bytes in the string
    [[nodiscard]] virtual size_t size() const;

    /// Get buffer pointer
    /// \return Buffer pointer
    [[nodiscard]] const char *data() const;

    /// Find substring, with index units in characters
    /// \param str Substring
    /// \return Substring position
    [[nodiscard]] int32_t find(const SStringView &str) const;

    /// Find substring, with index units in characters
    /// \param u8str Substring
    /// \return Substring position
    int32_t find(const char *u8str) const;

    /// Find byte string, with index units in bytes
    /// \param bytes Substring
    /// \return Substring position
    int32_t findByBytes(const char *bytes) const;

    /// Trim whitespace from both ends of the string
    /// \note Note that it's whitespace
    /// \return Processed object
    [[nodiscard]] SString trim() const;

    /// Reverse the string
    /// \return Reversed object
    [[nodiscard]] SString reverse() const;

    /// Append string to the end
    /// \param str String to append
    /// \return Resulting appended string
    [[nodiscard]] SString append(const SStringView &str) const;

    /// Append string to the end
    /// \param u8str String to append
    /// \return Resulting appended string
    /// \deprecated The appended string must also be UTF-8 encoded, otherwise use is not recommended
    SString append(const char *u8str) const;

    /// Split the string
    /// \param str Delimiter
    /// \return Split result
    [[nodiscard]] std::vector<SString> split(const SStringView &str) const;

    /// Split the string
    /// \param str Delimiter
    /// \return Split result
    std::vector<SString> split(const char *str) const;

    /// Extract substring [begin, len - 1]
    /// @param begin Starting index of the substring
    /// @return Substring
    [[nodiscard]] SString substring(size_t begin) const;
    /// \brief Extract substring [begin, begin + len - 1]
    /// \param begin Starting index of the substring
    /// \param len Length to extract
    /// \return Substring
    [[nodiscard]] SString substring(size_t begin, size_t len) const;

    /// \brief Check if the string ends with a given substring
    /// \param suffix Matching substring
    /// \return Result
    [[nodiscard]] bool endsWith(const SStringView &suffix) const;

    /// \brief Check if the string ends with a given substring
    /// \param suffix Matching substring
    /// \return Result
    [[nodiscard]] bool endsWith(const std::string_view &suffix) const;

    /// \brief Check if the string starts with a given substring
    /// \param prefix Matching substring
    /// \return Result
    [[nodiscard]] bool startsWith(const SStringView &prefix) const;

    /// \brief Check if the string starts with a given substring
    /// \param prefix Matching substring
    /// \return Result
    [[nodiscard]] bool startsWith(const std::string_view &prefix) const;

    /// Check if the letter is entirely lowercase
    [[nodiscard]] bool isLower() const;
    /// Check if the letter is entirely uppercase
    [[nodiscard]] bool isUpper() const;

    /// Create a copy with all letters converted to lowercase
    [[nodiscard]] SString toLower() const;
    /// Create a copy with all letters converted to uppercase
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

/// String
class SString final : public SStringView {
public:
    friend class SStringView;

    explicit SString() noexcept;
    SString(const char *str, size_t size);
    SString(const SString &s_string) noexcept;
    SString(SString &&s_string) noexcept;
    ~SString() noexcept override;

    static SString fromSChars(SChar ch[], size_t size);
    static SString fromSChars(std::vector<SChar> &chars);
    static SString fromUTF8(const char *str);
    static SString fromUCS2LE(const wchar_t *str);

public:
    /// Get buffer capacity
    /// \return Buffer capacity
    [[nodiscard]] size_t cap() const;
    /// Get used buffer size
    /// \return Used buffer size
    [[nodiscard]] size_t size() const override;

    /// Convert the string to lowercase
    void toLower();
    /// Convert the string to uppercase
    void toUpper();

    /// \brief Get data pointer
    /// \deprecated This function should generally not be used
    char *data();

public:
    void operator+=(const SStringView &str);
    void operator+=(const char *u8str);

protected:
    size_t _capacity = 0;
};
} // namespace sstr