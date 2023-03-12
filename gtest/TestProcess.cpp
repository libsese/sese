#include "sese/system/Process.h"
#include "gtest/gtest.h"

TEST(TestProcess, _0) {
    auto a = sese::Process::create("ls /home/kaoru -a");
    ASSERT_TRUE(a != nullptr);
    auto code = a->wait();
    ASSERT_TRUE(code == 0);
}