#include "sese/record/Marco.h"
#include "sese/system/Process.h"
#include "gtest/gtest.h"

TEST(TestProcess, EXEC_LS) {
#ifdef _WIN32
    char cmd[] = {"powershell ls \"C:/ \""};
#elif __linux__
    char cmd[] = {"ls \"/ \" -a"};
#elif __APPLE__
    char cmd[] = {"ls \"/ \""};
#endif

    auto a = sese::system::Process::create(cmd);
    ASSERT_TRUE(a != nullptr);
    auto pid = a->getProcessId();
    SESE_INFO("sub process pid is %d", pid);
    auto code = a->wait();
    ASSERT_TRUE(code == 0);
}

TEST(TestProcess, PID) {
    auto pid = sese::system::Process::getCurrentProcessId();
    EXPECT_NE(pid, 0);
    SESE_INFO("current pid is %d", pid);
}