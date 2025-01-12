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

#include <sese/system/FileNotifier.h>
#include <sese/system/ProcessBuilder.h>

#include <filesystem>
#include <thread>

#include "gtest/gtest.h"

#ifdef WIN32
#define PATH "C:/temp/"
#else
#define PATH "/tmp/"
#endif

using namespace std::chrono_literals;

TEST(TestFileNotifier, Notify) {
    std::filesystem::create_directory(PATH);
    auto notifier_or = sese::system::FileNotifier::createEx(PATH);
    if (notifier_or) {
        FAIL() << notifier_or.err().message();
    }
    auto &notifier = notifier_or.get();
    notifier->setOnCreate([](std::string_view name) {
        printf("touch %s\n", name.data());
    });
    notifier->setOnModify([](std::string_view name) {
        printf("mod %s\n", name.data());
    });
    notifier->setOnDelete([](std::string_view name) {
        printf("rm %s\n", name.data());
    });
    notifier->setOnMove([](std::string_view src_name, std::string_view dst_name) {
        printf("mv %s %s\n", src_name.data(), dst_name.data());
    });
    notifier->start();

    auto result = sese::system::ProcessBuilder(PY_EXECUTABLE)
        .arg(PROJECT_PATH "/scripts/change_file.py")
        .arg(PATH)
        .createEx();
    if (result) {
        notifier->shutdown();
        FAIL() << "failed to create process";
    }
    auto &process = result.get();
    EXPECT_EQ(process->wait(), 0);
    std::this_thread::sleep_for(2s);
    // notifier->shutdown();
}
