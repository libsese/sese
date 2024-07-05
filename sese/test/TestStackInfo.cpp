#include <sese/record/Marco.h>
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