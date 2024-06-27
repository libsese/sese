#include "sese/record/Marco.h"
#include "sese/system/Process.h"
#include "sese/system/ProcessBuilder.h"
#include "gtest/gtest.h"

TEST(TestProcess, EXEC_LS) {
#ifdef _WIN32
    char cmd[] = {"powershell ls \"C:/\""};
#elif __linux__
    char cmd[] = {"ls / -a"};
#elif __APPLE__
    char cmd[] = {"ls /"};
#endif

    auto process = sese::system::Process::create(cmd);
    ASSERT_TRUE(process != nullptr);
    auto pid = process->getProcessId();
    SESE_INFO("sub process pid is {}", pid);
    EXPECT_EQ(0, process->wait());
}

TEST(TestProcess, PID) {
    auto pid = sese::system::Process::getCurrentProcessId();
    EXPECT_NE(pid, 0);
    SESE_INFO("current pid is {}", pid);
}

#include "sese/system/CommandLine.h"

TEST(TestProcess, BUILDER) {
    using sese::system::CommandLine;
    using sese::system::ProcessBuilder;
    auto process = ProcessBuilder(CommandLine::getArgv()[0])
                           .args("--help")
                           .create();
    EXPECT_EQ(0, process->wait());
}