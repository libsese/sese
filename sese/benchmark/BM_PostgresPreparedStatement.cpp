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

using sese::text::StringBuilder;

const std::string STRING = "SELECT * FROM tb_xxx WHERE xxx = ? AND xxx = ?;";

static void FountPos() {
    StringBuilder builder;
    uint32_t count = 0;
    std::string::const_iterator begin = STRING.begin();
    while (true) {
        std::string::const_iterator pos = std::find(begin, STRING.end(), '?');
        builder.append(std::string{begin, pos});
        if (pos == STRING.end()) {
            break;
        }
        builder << '$' << std::to_string(++count);
        begin = pos + 1;
    }
}

static void BM_FountPos(benchmark::State &state) {
    for (auto _: state) {
        FountPos();
    }
}

static void Foreach() {
    StringBuilder builder;
    uint32_t count = 0;
    for (char i: STRING) {
        if (i == '?') {
            count++;
            builder << '$' << std::to_string(count);
        } else {
            builder << i;
        }
    }
}

static void BM_Foreach(benchmark::State &state) {
    for (auto _: state) {
        Foreach();
    }
}

BENCHMARK(BM_Foreach);
BENCHMARK(BM_FountPos);
BENCHMARK_MAIN();