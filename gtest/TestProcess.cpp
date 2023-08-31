#include "sese/system/Process.h"
#include "gtest/gtest.h"

TEST(TestProcess, _0) {
#ifdef _WIN32
    char cmd[] = {"powershell ls C:/"};
#elif __linux__
    char cmd[] = {"ls / -a"};
#elif __APPLE__
    char cmd[] = {"ls /"};
#endif

    auto a = sese::system::Process::create(cmd);
    ASSERT_TRUE(a != nullptr);
    auto code = a->wait();
    ASSERT_TRUE(code == 0);
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}