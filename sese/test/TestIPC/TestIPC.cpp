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

#include <gtest/gtest.h>
#include <sese/system/IPC.h>
#include <sese/system/Process.h>
#include <sese/record/Marco.h>

TEST(TestIPC, Temp) {
    auto channel = sese::system::IPCChannel::create("Test", 1024);
    ASSERT_TRUE(channel->write(5, "hello"));
    ASSERT_TRUE(channel->write(6, "abc"));
    ASSERT_TRUE(channel->write(5, "hi"));
    ASSERT_TRUE(channel->write(6, "bye"));

    {
        auto result = channel->read(5);
        for (auto &&msg: result) {
            SESE_INFO("message 5: %s", msg.getDataAsString().c_str());
        }
    }

    {
        auto result = channel->read(6);
        for (auto &&msg: result) {
            SESE_INFO("message 6: %s", msg.getDataAsString().c_str());
            SESE_INFO("message point %p", msg.data());
            SESE_INFO("message length %zu", msg.length());
        }
    }
}

TEST(TestIPC, Process) {
    auto channel = sese::system::IPCChannel::create("IPCTestForSese", 4096);
    ASSERT_NE(nullptr, channel);

    auto process = sese::system::Process::create(PATH_TO_IPC_D);
    ASSERT_NE(nullptr, process);

    channel->write(1, "Hello");
    channel->write(2, "Hi");
    channel->write(1, "12345", 4);
    channel->write(1, "Exit");

    EXPECT_EQ(0, process->wait());
    auto res = channel->read(2);
    EXPECT_EQ(res.front().getDataAsString(), "Hi");
}