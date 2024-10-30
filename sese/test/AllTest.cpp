#include <sese/util/Initializer.h>
#include <sese/system/Environment.h>

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::initCore(argc, argv);
    sese::sys::Environment::setEnv("LANG", "en_US.UTF-8");
    sese::sys::Environment::setEnv("LC_ALL", "en_US.UTF-8");
#if defined(__linux__) || defined(__APPLE__)
    signal(SIGPIPE, SIG_IGN);
#endif
    return RUN_ALL_TESTS();
}