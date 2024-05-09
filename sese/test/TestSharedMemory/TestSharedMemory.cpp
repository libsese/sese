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

    auto p_int = (int64_t *) mem->getBuffer();
    LogHelper::i("memory daemon init number is: %" PRId64, *p_int);

    *p_int += 1;
    EXPECT_EQ(process->wait(), 0);
}