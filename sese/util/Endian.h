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
 * @file Endian.h
 * @author kaoru
 * @brief Endianness conversion
 * @date April 3, 2022
 */

#pragma once

#include <type_traits>
#include <cstring>
#include <cstdint>

enum class EndianType {
    LITTLE,
    BIG
};

#define ByteSwap16(i16) ((i16 & 0x00FF) << 8 | (i16 & 0xFF00) >> 8)
#define ByteSwap32(i32) ((i32 & 0x000000FF) << 24 | (i32 & 0x0000FF00) << 8 | (i32 & 0x00FF0000) >> 8 | (i32 & 0xFF000000) >> 24)
#define ByteSwap64(i64)                        \
    ((i64 & 0x00000000000000FF) << 56 |         \
            (i64 & 0x000000000000FF00) << 40 | \
            (i64 & 0x0000000000FF0000) << 24 | \
            (i64 & 0x00000000FF000000) << 8 |  \
            (i64 & 0x000000FF00000000) >> 8 |  \
            (i64 & 0x0000FF0000000000) >> 24 | \
            (i64 & 0x00FF000000000000) >> 40 | \
            (i64 & 0xFF00000000000000) >> 56)

extern "C++" {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32)
#define SESE_LITTLE_ENDIAN
template<typename T>
inline T ToBigEndian16(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline T ToBigEndian32(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline T ToBigEndian64(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T ToLittleEndian16(T t) {
    return t;
}

template<typename T>
inline T ToLittleEndian32(T t) {
    return t;
}

template<typename T>
inline T ToLittleEndian64(T t) {
    return t;
}

template<typename T>
inline T FromBigEndian16(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline T FromBigEndian32(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline T FromBigEndian64(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T FromLittleEndian16(T t) {
    return t;
}

template<typename T>
inline T FromLittleEndian32(T t) {
    return t;
}

template<typename T>
inline T FromLittleEndian64(T t) {
    return t;
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 2, T>::t
        ToBigEndian(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 4, T>::t
        ToBigEndian(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 8, T>::t
        ToBigEndian(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T ToLittleEndian(T t) {
    return t;
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 2, T>::t
        FromBigEndian(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 4, T>::t
        FromBigEndian(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 8, T>::t
        FromBigEndian(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T FromLittleEndian(T t) {
    return t;
}

#else
#define SESE_BIG_ENDIAN
template<typename T>
inline T ToBigEndian16(T t) {
    return t;
}

template<typename T>
inline T ToBigEndian32(T t) {
    return t;
}

template<typename T>
inline T ToBigEndian64(T t) {
    return t;
}

template<typename T>
inline T ToLittleEndian16(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline T ToLittleEndian32(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline T ToLittleEndian64(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T FromBigEndian16(T t) {
    return t;
}

template<typename T>
inline T FromBigEndian32(T t) {
    return t;
}

template<typename T>
inline T FromBigEndian64(T t) {
    return t;
}

template<typename T>
inline T FromLittleEndian16(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline T FromLittleEndian32(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline T FromLittleEndian64(T t) {
    return ByteSwap64(t);
}


template<typename T>
inline
        typename std::enable_if<sizeof(T) == 2, T>::t
        ToLittleEndian(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 4, T>::t
        ToLittleEndian(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 8, T>::t
        ToLittleEndian(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T ToBigEndian(T t) {
    return t;
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 2, T>::t
        FromLittleEndian(T t) {
    return ByteSwap16(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 4, T>::t
        FromLittleEndian(T t) {
    return ByteSwap32(t);
}

template<typename T>
inline
        typename std::enable_if<sizeof(T) == 8, T>::t
        FromLittleEndian(T t) {
    return ByteSwap64(t);
}

template<typename T>
inline T FromBigEndian(T t) {
    return t;
}
#endif

#define ASSERT_FLOAT_SIZE static_assert(sizeof(float) == 4, "Float size is not 4 bytes")
#define ASSERT_DOUBLE_SIZE static_assert(sizeof(double) == 8, "Double size is not 8 bytes")

template<>
inline float ToBigEndian32(float t) {
    ASSERT_FLOAT_SIZE;
#ifdef SESE_LITTLE_ENDIAN
    int32_t tmp;
    memcpy(&tmp, &t, sizeof(float));
    ToBigEndian32<int32_t>(tmp);
    memcpy(&t, &tmp, sizeof(float));
#endif
    return t;
}

template<>
inline double ToBigEndian64(double t) {
    ASSERT_DOUBLE_SIZE;
#ifdef SESE_LITTLE_ENDIAN
    int64_t tmp;
    memcpy(&tmp, &t, sizeof(double));
    ToBigEndian64<int64_t>(tmp);
    memcpy(&t, &tmp, sizeof(double));
#endif
    return t;
}

template<>
inline float ToLittleEndian32(float t) {
    ASSERT_FLOAT_SIZE;
#ifdef SESE_BIG_ENDIAN
    int32_t tmp;
    memcpy(&tmp, &t, sizeof(float));
    ToBigEndian32<int32_t>(tmp);
    memcpy(&t, &tmp, sizeof(float));
#endif
    return t;
}

template<>
inline double ToLittleEndian64(double t) {
    ASSERT_DOUBLE_SIZE;
#ifdef SESE_BIG_ENDIAN
    int64_t tmp;
    memcpy(&tmp, &t, sizeof(double));
    ToBigEndian64<int64_t>(tmp);
    memcpy(&t, &tmp, sizeof(double));
#endif
    return t;
}

template<>
inline float FromBigEndian32(float t) {
    ASSERT_FLOAT_SIZE;
#ifdef SESE_LITTLE_ENDIAN
    int32_t tmp;
    memcpy(&tmp, &t, sizeof(float));
    FromBigEndian32<int32_t>(tmp);
    memcpy(&t, &tmp, sizeof(float));
#endif
    return t;
}

template<>
inline double FromBigEndian64(double t) {
    ASSERT_DOUBLE_SIZE;
#ifdef SESE_LITTLE_ENDIAN
    int64_t tmp;
    memcpy(&tmp, &t, sizeof(double));
    FromBigEndian64<int64_t>(tmp);
    memcpy(&t, &tmp, sizeof(double));
#endif
    return t;
}

template<>
inline float FromLittleEndian32(float t) {
    ASSERT_FLOAT_SIZE;
#ifdef SESE_BIG_ENDIAN
    int32_t tmp;
    memcpy(&tmp, &t, sizeof(float));
    FromBigEndian32<int32_t>(tmp);
    memcpy(&t, &tmp, sizeof(float));
#endif
    return t;
}

template<>
inline double FromLittleEndian64(double t) {
    ASSERT_DOUBLE_SIZE;
#ifdef SESE_BIG_ENDIAN
    int64_t tmp;
    memcpy(&tmp, &t, sizeof(double));
    FromBigEndian64<int64_t>(tmp);
    memcpy(&t, &tmp, sizeof(double));
#endif
    return t;
}

#undef ASSERT_FLOAT_SIZE
#undef ASSERT_DOUBLE_SIZE
}