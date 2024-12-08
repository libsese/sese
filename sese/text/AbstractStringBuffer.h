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

/**
 * @file AbstractStringBuffer.h
 * @brief String buffer class
 * @author kaoru
 * @date March 28, 2022
 * @version 0.2
 */

#pragma once

#include "sese/Config.h"
#include "sese/text/String.h"

#include <vector>

namespace sese::text {

/**
 * @brief String buffer class
 */
class  AbstractStringBuffer {
public:
    /**
     * @param cap Initial capacity
     */
    explicit AbstractStringBuffer(size_t cap = STRING_BUFFER_SIZE_FACTOR) noexcept;
    /**
     * @brief Initialize with a string
     * @param str String
     */
    explicit AbstractStringBuffer(const char *str) noexcept;
    virtual ~AbstractStringBuffer() noexcept;

    AbstractStringBuffer(const AbstractStringBuffer &abstract_string_buffer) noexcept;
    AbstractStringBuffer(AbstractStringBuffer &&abstract_string_buffer) noexcept;

    static std::vector<std::string> split(const std::string_view &text, const std::string_view &sub) noexcept;
    static bool startsWith(const std::string_view &text, const std::string_view &prefix) noexcept;
    static bool endsWith(const std::string_view &text, const std::string_view &suffix) noexcept;

protected:
    size_t cap{};           /// Actual capacity
    size_t len = 0;         /// String length
    char *buffer = nullptr; /// String caching

    /**
     * Expansion
     * @param new_size Size after expansion
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
    /// Remove whitespace at both ends
    virtual void trim() noexcept;
    [[nodiscard]] virtual std::vector<std::string> split(const std::string_view &str) const noexcept;
    [[nodiscard]] virtual bool startsWith(const std::string_view &prefix) const noexcept;
    [[nodiscard]] virtual bool endsWith(const std::string_view &suffix) const noexcept;
    virtual std::string toString();
    virtual String toSString();
    virtual void *buf();
};

} // namespace sese::text