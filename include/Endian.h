/**
 * @file Endian.h
 * @author kaoru
 * @brief 字节序转换
 * @date 2022年4月3日
 */

#pragma once

#define ByteSwap16(i16) (i16 & 0x00FF) << 8 | (i16 & 0xFF00) >> 8
#define ByteSwap32(i32) (i32 & 0x000000FF) << 24 | (i32 & 0x0000FF00) << 8 | (i32 & 0x00FF0000) >> 8 | (i32 & 0xFF000000) >> 24
#define ByteSwap64(i64)                        \
    (i64 & 0x00000000000000FF) << 56 |         \
            (i64 & 0x000000000000FF00) << 40 | \
            (i64 & 0x0000000000FF0000) << 24 | \
            (i64 & 0x00000000FF000000) << 8 |  \
            (i64 & 0x000000FF00000000) >> 8 |  \
            (i64 & 0x0000FF0000000000) >> 24 | \
            (i64 & 0x00FF000000000000) >> 40 | \
            (i64 & 0xFF00000000000000) >> 56

extern "C++" {
#if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32)
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
#else
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
#endif
}