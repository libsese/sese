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
 * @file Util.h
 * @brief Miscellaneous utility functions
 * @author kaoru
 * @date March 28, 2022
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace sese {

enum class Level;

/// Integer conversion function that ignores errors
/// @param string String representing the integer
/// @param radix Base
/// @return Integer
int64_t toInteger(const std::string &string, int radix = 10);

/// Get the name of the class
/// @param type Class
/// @return Name of the class
std::string getClassName(const std::type_info *type);

/**
 * @brief Character comparator
 */
struct StrCmp {
    int operator()(char const *lv, char const *rv) const;
    int operator()(const std::string &lv, const std::string &rv) const;
};

/**
 * @brief Character comparator (case-insensitive)
 */
struct StrCmpI {
    int operator()(char const *lv, char const *rv) const;
    int operator()(const std::string &lv, const std::string &rv) const;
};

/**
 * Compare two strings for equality
 * \param lv String 1
 * \param rv String 2
 * \return Comparison result
 */
bool strcmp(char const *lv, char const *rv) noexcept;

/**
 * Compare two strings for equality (case-insensitive)
 * \param lv String 1
 * \param rv String 2
 * \return Comparison result
 */
bool strcmpDoNotCase(char const *lv, char const *rv) noexcept;

/**
 * Determine if a character is a whitespace
 * @param ch Character to be checked
 * @return Whether it is a whitespace
 */
bool isSpace(char ch) noexcept;

/**
 * Return corresponding string based on log level
 * @param level Log level
 * @return String
 */
const char *getLevelString(Level level) noexcept;

/**
 * Find the first occurrence of a character in a string
 * @param str The string to search in
 * @param ch The character to find
 * @return The position of the first occurrence, or -1 if not found
 */
int32_t findFirstAt(const char *str, char ch);

/**
 * Make the current thread sleep for a while, implemented using NATIVE API
 * @param second Seconds
 */
void sleep(uint32_t second);

/**
 * Make the current thread sleep for a while, implemented using std::this_thread::sleep_for
 * @param duration The duration to sleep
 */
template<class REP, class PERIOD>
void sleep(const std::chrono::duration<REP, PERIOD> &duration) {
    std::this_thread::sleep_for(duration);
}

/**
 * Retrieve error message
 * @param error Error code
 * @return Error description
 */
std::string getErrorString(int64_t error = errno);

/**
 * Retrieve error code
 * @return Error code
 */
int32_t  getErrorCode();

/// Move stream data
/// \param out Output stream
/// \param in Input stream
/// \param size Size to move
/// \return Actual size moved
size_t streamMove(sese::io::OutputStream *out, sese::io::InputStream *in, size_t size) noexcept;

/// Get the byte length required to convert a number to a string
/// @note Only integers can be converted
/// @tparam T Input integer type
/// @param num Number to be converted
/// @param radix Base
/// @return Length required for the number string
template<class T>
size_t number2StringLength(T num, size_t radix = 10) {
    static_assert(!std::is_same_v<T, double>, "Must be an integer");
    static_assert(!std::is_same_v<T, float>, "Must be an integer");
    size_t length = 0;
    if (num == 0) return 1;
    if (num < 0) {
        length += 1;
        num *= -1;
    }
    do {
        num /= static_cast<T>(radix);
        length += 1;
    } while (num > 0);
    return length;
}

/// Get the byte length required to convert a number to a string
/// @note Only floating-point numbers can be converted
/// @tparam T Input floating-point number type
/// @param num Number to be converted
/// @param placeholder Precision
/// @return Length required for the number string
template<class T>
size_t floating2StringLength(T num, uint16_t placeholder) {
    static_assert(std::is_same_v<T, double> || std::is_same_v<T, float>, "Must be a floating number");
    size_t length = number2StringLength(static_cast<int64_t>(num));
    if (placeholder > 0) {
        length += 1 + placeholder;
    }
    return length;
}

// GCOVR_EXCL_START
/// Time type conversion
/// @see https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
/// @tparam TP Type of the object to convert
/// @param tp Object to convert
/// @return Time in std::time_t type
template<typename TP>
std::time_t to_time_t(TP tp) {
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}
// GCOVR_EXCL_STOP

 /// Additive spill detection
template <typename T>
bool isAdditionOverflow(T a, T b) {
    if constexpr (std::is_unsigned_v<T>) {
        return a > std::numeric_limits<T>::max() - b;
    } else {
        if ((b > 0 && a > std::numeric_limits<T>::max() - b) ||
            (b < 0 && a < std::numeric_limits<T>::min() - b)) {
            return true;
        }
    }
    return false;
}

/// Subtractive spill detection
template <typename T>
bool isSubtractionOverflow(T a, T b) {
    if constexpr (std::is_unsigned<T>::value) {
        return a < b;
    } else {
        if ((b < 0 && a > std::numeric_limits<T>::max() + b) ||
            (b > 0 && a < std::numeric_limits<T>::min() + b)) {
           return true;
        }
    }
    return false;
}

} // namespace sese

/**
 * Retrieve detailed information (C interface)
 * @return Version information string
 */
extern "C" const char *getSpecificVersion();

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

/**
 * @brief Faster boundary check - [0, max)
 */
#define CheckRange(x, max) ((unsigned int) (x) < (max))

/**
 * @brief Faster boundary check - [min, max]
 */
#define CheckRangeBetween(x, min, max) (((x) - (min)) | ((max) - (x)) >= 0)