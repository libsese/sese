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

#pragma once

#include <sese/types/defines.h>

namespace sese {

template<typename T, typename = void>
struct has_begin : std::false_type {}; // NOLINT

template<typename T>
struct has_begin<T, void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

template<typename T, typename = void>
struct has_end : std::false_type {}; // NOLINT

template<typename T>
struct has_end<T, void_t<decltype(std::declval<T>().begin())>> : std::true_type {};

template <typename T>
struct is_iterable : std::conjunction<has_begin<T>, has_end<T>> {}; // NOLINT

template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value; // NOLINT

} // namespace sese