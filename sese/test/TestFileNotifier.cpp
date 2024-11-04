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
#include <sese/system/Process.h>

#include <thread>

#include "gtest/gtest.h"

#ifdef WIN32
#define PATH "C:/temp/"
#else
#define PATH "/tmp/"
#endif

using namespace std::chrono_literals;

class Option : public sese::system::FileNotifyOption {
public:
    void onCreate(std::string_view name) override {
        printf("touch %s\n", name.data());
    }
    void onMove(std::string_view src_name, std::string_view dst_name) override {
        printf("mv %s %s\n", src_name.data(), dst_name.data());
    }
    void onModify(std::string_view name) override {
        printf("mod %s\n", name.data());
    }
    void onDelete(std::string_view name) override {
        printf("rm %s\n", name.data());
    }
};

TEST(TestFileNotifier, Notify) {
    Option option{};
    auto notifier = sese::system::FileNotifier::create(PATH, &option);
    notifier->loopNonblocking();

    auto cmd = PY_EXECUTABLE " " PROJECT_PATH "/scripts/change_file.py " PATH;
    auto process = sese::system::Process::create(cmd);
    if (process == nullptr) {
        notifier->shutdown();
        FAIL() << "failed to create process";
    }
    EXPECT_EQ(process->wait(), 0);
    std::this_thread::sleep_for(2s);
    // notifier->shutdown();
}
