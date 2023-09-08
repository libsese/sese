#include <sese/record/Marco.h>
#include <sese/system/StackInfo.h>
#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

void function1() {
    sese::system::StackInfo stacks(3, 1);
    for (auto &&item : stacks) {
        SESE_INFO("%s at %" PRId64, item.func.c_str(), item.address);
        // SESE_INFO("\t%s:%" PRId64, item.file.c_str(), item.line);
    }
    SESE_DEBUG("end;");
}

extern "C" void function2() {
    sese::system::StackInfo stacks(5);
    for (auto &&item : stacks) {
        SESE_INFO("%s at %" PRId64, item.func.c_str(), item.address);
        // SESE_INFO("\t%s:%" PRId64, item.file.c_str(), item.line);
    }
    SESE_DEBUG("end;");
}

TEST(TestStackInfo, _0) {
    function1();
    function2();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}