#include "sese/system/Process.h"
#include "gtest/gtest.h"

TEST(TestProcess, _0) {
#ifdef _WIN32
    char cmd[] = {"powershell ls C:/"};
#else
    char cmd[] = {"ls / -a"};
#endif

    auto a = sese::Process::create(cmd);
    ASSERT_TRUE(a != nullptr);
    auto code = a->wait();
    ASSERT_TRUE(code == 0);
}