#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::initCore(argc, argv);
#if defined(__linux__) || defined(__APPLE__)
    signal(SIGPIPE, SIG_IGN);
#endif
    return RUN_ALL_TESTS();
}