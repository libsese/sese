#pragma once

#include <sese/util/Stream.h>

#include <array>
#include <bitset>
#include <string>
#include <vector>

#define SESE_OUT_TYPE_DEF(type, data) int64_t operator<<(sese::OutputStream &out, type data)
#define SESE_OUT_PTR_TYPE_DEF(type, data) int64_t operator<<(sese::OutputStream *out, type data)
#define SESE_OUT_UPTR_TYPE_DEF(type, data) int64_t operator<<(const std::unique_ptr<sese::OutputStream> &out, type data)
#define SESE_OUT_SPTR_TYPE_DEF(type, data) int64_t operator<<(const std::shared_ptr<sese::OutputStream> &out, type data)

#define SESE_OUT_FUNC_BODY(ptr, len) \
    { return out.write(ptr, len); }
#define SESE_OUT_PTR_FUNC_BODY(ptr, len) \
    { return out->write(ptr, len); }


#define SESE_OUT_DEF(type, data, ptr, len) \
    SESE_OUT_TYPE_DEF(type, data)          \
    SESE_OUT_FUNC_BODY(ptr, len)

#define SESE_OUT_DEF_ALL(type, data, ptr, len) \
    SESE_OUT_TYPE_DEF(type, data)              \
    SESE_OUT_FUNC_BODY(ptr, len)               \
    SESE_OUT_PTR_TYPE_DEF(type, data)          \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)           \
    SESE_OUT_UPTR_TYPE_DEF(type, data)         \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)           \
    SESE_OUT_SPTR_TYPE_DEF(type, data)         \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)

#define SESE_OUT_DEF_TEMPLATE(data, ptr, len, type, ...) \
    template<__VA_ARGS__>                                \
    SESE_OUT_TYPE_DEF(type, data)                        \
    SESE_OUT_FUNC_BODY(ptr, len)

#define SESE_OUT_DEF_TEMPLATE_ALL(data, ptr, len, type, ...) \
    template<__VA_ARGS__>                                    \
    SESE_OUT_TYPE_DEF(type, data)                            \
    SESE_OUT_FUNC_BODY(ptr, len)                             \
    template<__VA_ARGS__>                                    \
    SESE_OUT_PTR_TYPE_DEF(type, data)                        \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)                         \
    template<__VA_ARGS__>                                    \
    SESE_OUT_UPTR_TYPE_DEF(type, data)                       \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)                         \
    template<__VA_ARGS__>                                    \
    SESE_OUT_SPTR_TYPE_DEF(type, data)                       \
    SESE_OUT_PTR_FUNC_BODY(ptr, len)

SESE_OUT_DEF(char, data, &data, sizeof(char))
SESE_OUT_DEF(const char *, data, data, strlen(data))

SESE_OUT_DEF_TEMPLATE_ALL(data, data.c_str(), sizeof(T) * data.size(), std::basic_string<T>, class T)
SESE_OUT_DEF_TEMPLATE_ALL(data, data.data(), sizeof(T) * data.length(), std::basic_string_view<T>, class T)
SESE_OUT_DEF_TEMPLATE_ALL(data, data.data(), sizeof(T) * data.size(), std::vector<T>, class T)
SESE_OUT_DEF_TEMPLATE_ALL(data, data.to_string().c_str(), data.size(), std::bitset<N>, size_t N)

template<class T, size_t N>
int64_t operator<<(sese::OutputStream &output, std::array<T, N> data) {
    return output.write(data.data(), sizeof(T) * N);
}

template<class T, size_t N>
int64_t operator<<(sese::OutputStream *output, std::array<T, N> data) {
    return output->write(data.data(), sizeof(T) * N);
}

#if SESE__CXX_STANDARD > 201703L
#include <span>
SESE_OUT_DEF_TEMPLATE_ALL(data, data.data(), sizeof(T) * data.size(), std::span<T>, class T)
#endif

#include "sese/text/String.h"

SESE_OUT_DEF_ALL(const sese::text::StringView &, data, data.data(), data.len())
SESE_OUT_DEF_ALL(const sese::text::String &, data, ((sese::text::StringView *) &data)->data(), data.len())