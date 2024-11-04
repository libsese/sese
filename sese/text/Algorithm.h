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

/// \file algorithm.h
/// \date 2022-9-27
/// \version 0.1
/// \author kaoru
/// \brief 库所需的外部算法，查找、位移等

#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace sstr {

struct SChar;

typedef int (*FindFunction)(const char *, const char *);

extern int KMP(const char *str, const char *sub);

extern int BM(const char *str, const char *sub);

extern int BM(const uint32_t *str, size_t size, std::vector<SChar> &sub);

extern int NORMAL(const char *str, const char *sub);

// GCOVR_EXCL_START

/// 对目标缓存的元素左移
/// \warning 使用时务必判断数组是否可能越界
/// \tparam T 元素类型
/// \param header 目标缓存
/// \param len 目标缓存原始大小（已经使用的）
/// \param begin 操作起始位置
/// \param count 移动距离
template<typename T>
inline void LeftShiftElement(T *header, size_t len, size_t begin, size_t count) {
    for (size_t i = 0; i < len - begin; i++) {
        header[begin + i] = header[begin + count + i];
    }
}

/// 对目标缓存的元素左移
/// \warning 使用时务必判断数组是否可能越界
/// \tparam T 元素类型
/// \param header 目标缓存
/// \param len 目标缓存原始大小（已经使用的）
/// \param begin 操作起始位置
/// \param count 移动距离
template<typename T>
inline void RightShiftElement(T *header, size_t len, size_t begin, size_t count) {
    for (size_t i = 0; i < len - begin; i++) {
        header[len - 1 + count - i] = header[len - 1 - i];
    }
}

// GCOVR_EXCL_STOP

} // namespace sstr
