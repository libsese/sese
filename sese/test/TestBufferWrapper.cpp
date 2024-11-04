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

#include "sese/io/InputBufferWrapper.h"
#include "sese/io/OutputBufferWrapper.h"

#include <gtest/gtest.h>

TEST(TestBufferWrapper, Operator) {
    char buffer0[]{"Hello world"};
    char buffer1[8]{};

    auto input = sese::io::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);
    auto output = sese::io::OutputBufferWrapper(buffer1, sizeof(buffer1));

    auto len = output << input;
    ASSERT_EQ(len, 8);

    output.reset();
    len = &output << input;
    ASSERT_EQ(len, 3);
}

TEST(TestBufferWrapper, Peek) {
    char buffer0[]{"Hello World"};
    auto input = sese::io::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);

    char buffer1[16]{};
    auto len = input.peek(buffer1, 16);
    len = input.trunc(16);
    ASSERT_EQ(len, 11);
    input.reset();
    len = input.peek(buffer1, len);
    len = input.trunc(len);
    ASSERT_EQ(len, 11);
    len = input.peek(buffer1, 0);
    len = input.trunc(0);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferWrapper, Read) {
    char buffer0[]{"Hello World"};
    auto input = sese::io::InputBufferWrapper(buffer0, sizeof(buffer0) - 1);

    char buffer1[16]{};
    auto len = input.read(buffer1, 16);
    ASSERT_EQ(len, 11);
    input.reset();
    len = input.read(buffer1, len);
    ASSERT_EQ(len, 11);
    len = input.read(buffer1, 0);
    ASSERT_EQ(len, 0);
}

TEST(TestBufferWrapper, Misc) {
    char buffer[8]{};
    auto output = sese::io::OutputBufferWrapper(buffer, sizeof(buffer));
    ASSERT_EQ(output.getBuffer(), buffer);
    ASSERT_EQ(output.getCapacity(), 8);
    ASSERT_EQ(output.getLength(), 0);
}