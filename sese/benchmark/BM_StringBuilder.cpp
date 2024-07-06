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

#include <benchmark/benchmark.h>
#include <sese/text/StringBuilder.h>
#include <sstream>

using sese::text::StringBuilder;
using std::stringstream;

constexpr auto SZ = 16 * 1024;
constexpr auto TIMES = 1000;

static void BM_StringBuilder(benchmark::State &state) {
    for (auto _: state) {
        StringBuilder builder(SZ);
        for (int i = 0; i < TIMES; ++i) {
            builder << "Number:" << std::to_string(i) << ";";
        }
    }
}

static void BM_StringStream(benchmark::State &state) {
    for (auto _: state) {
        stringstream stream;
        for (int i = 0; i < TIMES; ++i) {
            stream << "Number:" << std::to_string(i) << ";";
        }
    }
}

BENCHMARK(BM_StringBuilder);
BENCHMARK(BM_StringStream);
BENCHMARK_MAIN();