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

#include <simdjson.h>
#include <benchmark/benchmark.h>
#include <sese/config/Json.h>
#include <sese/io/InputBufferWrapper.h>

auto json_string = R"(
{
    "string": "a testing\n text\"",
    "value": 0,
    "nullable": null,
    "boolean": true,
    "object": {
        "object_value1": "value1",
        "object_value2": "value2"
    },
    "mixin_array": [
        1,
        2,
        {
            "object_value1": "value3",
            "object_value2": "value4"
        },
        3,
        [
            4,
            5
        ],
        null
    ]
}
)";

static void BM_BuiltinAlgo(benchmark::State &state) {
    sese::io::InputBufferWrapper wrapper(json_string, strlen(json_string));
    for (auto _: state) {
        auto v = sese::Json::parse(&wrapper, 8);
        benchmark::DoNotOptimize(v);
    }
}

static void BM_SimdAlgo(benchmark::State &state) {
    sese::io::InputBufferWrapper wrapper(json_string, strlen(json_string));
    for (auto _: state) {
        auto v = sese::Json::simdParse(&wrapper);
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(BM_BuiltinAlgo);
BENCHMARK(BM_SimdAlgo);
BENCHMARK_MAIN();