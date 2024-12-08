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
/// \brief External algorithms needed by the library, such as searching, shifting, etc.

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

/// Shift elements of the target buffer to the left
/// \warning Ensure to check for possible array bounds overflow when using
/// \tparam T Element type
/// \param header Target buffer
/// \param len Original size of the target buffer (already used)
/// \param begin Starting position of the operation
/// \param count Shift distance
template<typename T>
inline void LeftShiftElement(T *header, size_t len, size_t begin, size_t count) {
    for (size_t i = 0; i < len - begin; i++) {
        header[begin + i] = header[begin + count + i];
    }
}

/// Shift elements of the target buffer to the left
/// \warning Ensure to check for possible array bounds overflow when using
/// \tparam T Element type
/// \param header Target buffer
/// \param len Original size of the target buffer (already used)
/// \param begin Starting position of the operation
/// \param count Shift distance
template<typename T>
inline void RightShiftElement(T *header, size_t len, size_t begin, size_t count) {
    for (size_t i = 0; i < len - begin; i++) {
        header[len - 1 + count - i] = header[len - 1 - i];
    }
}

// GCOVR_EXCL_STOP

} // namespace sstr
