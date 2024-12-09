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

#include "Init.h"
#include <type_traits>

namespace sese {
/**
 * @brief Invokes the provided entry function with appropriate arguments.
 *
 * This template function checks the invocability of the provided function
 * object `func`. If `func` can be invoked without arguments, it calls
 * `func()`; if `func` can be invoked with `int` and `char**` arguments,
 * it calls `func(argc, argv)`.
 *
 * @tparam F Type of the function object.
 * @param func The function object to be invoked.
 * @param argc The argument count, typically from command line.
 * @param argv The argument vector, typically from command line.
 * @return The return value of the invoked function.
 *
 * @note The function object `func` must be invocable with either no arguments
 * or with `(int, char**)`. Otherwise, a static assertion will be triggered.
 */
template<typename F>
int invokeEntry(F &&func, int argc, char **argv) {
    if constexpr (std::is_invocable_v<F>) {
        return func();
    } else if constexpr (std::is_invocable_v<F, int, char **>) {
        return func(argc, argv);
    } else {
        static_assert(false, "Entry function must be invocable with () or (int, char**)");
        return -1;
    }
}

} // namespace sese

#define SESE_ENTER(EntryFunction)                            \
    int main(int argc, char **argv) {                        \
        sese::initCore(argc, argv);                          \
        return sese::invokeEntry(EntryFunction, argc, argv); \
    }
