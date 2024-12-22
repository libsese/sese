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

#include "sese/record/Marco.h"
#include "sese/system/Process.h"
#include "sese/system/ProcessBuilder.h"
#include "gtest/gtest.h"

TEST(TestProcess, EXEC_LS) {
#ifdef _WIN32
    char cmd[] = {"powershell ls \"C:/\""};
#elif __linux__
    char cmd[] = {"ls"};
#elif __APPLE__
    char cmd[] = {"ls"};
#endif

    auto result = sese::system::Process::createEx(cmd);
    ASSERT_FALSE(result) << result.err().message();
    auto &process = result.get();
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
    auto result_process = ProcessBuilder(CommandLine::getArgv()[0])
                           .arg("--help")
                           .createEx();
    EXPECT_EQ(0, result_process.get()->wait());
}

TEST(TestProcess, RESULT) {
    auto result = sese::sys::Process::createEx("undef.exe");
    if (result) {
        SESE_ERROR("{}", result.err().message());
        return;
    }
    auto &process = result.get();
}