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

#include <sese/system/SharedMemory.h>
#include <sese/system/Process.h>
#include <sese/record/LogHelper.h>
#include <sese/util/Util.h>

#include <gtest/gtest.h>

using sese::record::LogHelper;
using namespace std::chrono_literals;

TEST(TestSharedMemory, MEM_D) {
    auto process = sese::system::Process::create(PATH_TO_MEM_D);
    ASSERT_NE(process, nullptr) << "failed to start memory daemon: " << sese::getErrorString();

    std::this_thread::sleep_for(500ms);
    auto mem = sese::system::SharedMemory::use("SharedMemoryForSese");
    if (mem == nullptr) {
        EXPECT_TRUE(process->kill());
    }
    ASSERT_NE(mem, nullptr) << "failed to use shared memory: " << sese::getErrorString();

    auto p_int = static_cast<int64_t *>(mem->getBuffer());
    LogHelper::i("memory daemon init number is: %" PRId64, *p_int);

    *p_int += 1;
    EXPECT_EQ(process->wait(), 0);
}