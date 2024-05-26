#include <sese/record/Marco.h>
#include <sese/system/StackInfo.h>
#include <sese/text/Number.h>
#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

SESE_ALWAYS_INLINE void printStack(int skip) {
    sese::system::StackInfo stacks(5, skip);
    for (auto &&item: stacks) {
        SESE_INFO("%s at 0x%s", item.func.c_str(), sese::text::Number::toHex(item.address, false).c_str());
    }
    SESE_DEBUG("end;");
}

void function1() {
    printStack(sese::system::StackInfo::getSkipOffset());
}

extern "C" void function2() {
    printStack(sese::system::StackInfo::getSkipOffset());
}

namespace NamespaceStackInfo {
class ClassA {
public:
    ClassA() {
        printStack(sese::system::StackInfo::getSkipOffset());
    }

    virtual ~ClassA() {
        printStack(sese::system::StackInfo::getSkipOffset());
    }

    void hello() {
        printStack(sese::system::StackInfo::getSkipOffset());
    }

    void hello(int i) {
        printStack(sese::system::StackInfo::getSkipOffset());
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