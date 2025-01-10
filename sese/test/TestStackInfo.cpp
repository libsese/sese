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

#include <sese/log/Marco.h>
#include <sese/system/StackInfo.h>
#include <sese/text/Number.h>
#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

SESE_ALWAYS_INLINE void printStack(uint16_t skip) {
    sese::system::StackInfo stacks(5, skip);
    for (auto &&item: stacks) {
        SESE_INFO("{} at 0x{:x}", item.func, item.address);
    }
    SESE_DEBUG("end;");
}

void function1() {
    printStack(0);
}

extern "C" void function2() {
    printStack(0);
}

namespace NamespaceStackInfo {
class ClassA {
public:
    ClassA() {
        printStack(0);
    }

    virtual ~ClassA() {
        printStack(0);
    }

    void hello() {
        printStack(0);
    }

    void hello(int i) {
        printStack(0);
    }
};
} // namespace NamespaceStackInfo

TEST(TestStackInfo, Normal) {
    function1();
    function2();
}

TEST(TestStackInfo, Class) {
    {
        NamespaceStackInfo::ClassA a;
        a.hello();
        a.hello(1);
    }
}