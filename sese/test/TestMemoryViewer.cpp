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

#include <gtest/gtest.h>

#include <sese/util/MemoryViewer.h>
#include "sese/io/ConsoleOutputStream.h"

TEST(TestMemoryViewer, Output) {
    int8_t i8 = 20;
    int16_t i16 = 40;
    int32_t i32 = 90;
    int64_t i64 = 120;

    sese::io::ConsoleOutputStream output;

    sese::MemoryViewer::peer8(&output, &i8, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer16(&output, &i16, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer32(&output, &i32, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer64(&output, &i64, true);
    output.write("\n", 1);

    sese::MemoryViewer::peer16(&output, &i16, EndianType::BIG, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer16(&output, &i16, EndianType::LITTLE, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer32(&output, &i32, EndianType::BIG, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer32(&output, &i32, EndianType::LITTLE, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer64(&output, &i64, EndianType::BIG, true);
    output.write("\n", 1);
    sese::MemoryViewer::peer64(&output, &i64, EndianType::LITTLE, true);
    output.write("\n", 1);
}