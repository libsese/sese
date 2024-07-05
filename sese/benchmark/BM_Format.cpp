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
#include <sese/text/Format.h>
#include <sese/util/DateTime.h>

constexpr auto PI = 3.14159265;
constexpr size_t SZ = 1024;
const std::string STR = "Format";

static void BM_FormatFloat(benchmark::State &state) {
    for (auto _: state) {
        std::string str = sese::text::fmt("{:.3f}", PI);
        benchmark::DoNotOptimize(str);
    }
}

static void BM_SnprintfFloat(benchmark::State &state) {
    for (auto _: state) {
        auto buf = std::make_unique<char[]>(SZ);
        sese::text::snprintf(buf.get(), SZ, "%.3f", PI);
        benchmark::DoNotOptimize(buf);
    }
}


static void BM_Format(benchmark::State &state) {
    for (auto _: state) {
        auto date = sese::DateTime::now();
        std::string str = sese::text::fmt("{} {} {} {}", STR, date.getYears(), date.getMonths(), date.getDays());
        benchmark::DoNotOptimize(str);
    }
}

static void BM_Snprintf(benchmark::State &state) {
    for (auto _: state) {
        auto date = sese::DateTime::now();
        auto buf = std::make_unique<char[]>(SZ);
        sese::text::snprintf(buf.get(), SZ, "%s %d %d %d", STR.c_str(), date.getYears(), date.getMonths(), date.getDays());
        benchmark::DoNotOptimize(buf);
    }
}

BENCHMARK(BM_FormatFloat);
BENCHMARK(BM_SnprintfFloat);
BENCHMARK(BM_Format);
BENCHMARK(BM_Snprintf);
BENCHMARK_MAIN();
