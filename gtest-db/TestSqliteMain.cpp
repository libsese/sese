#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>
#include <gtest/gtest.h>

#include "Config.h"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::initCore(argc, argv);
    SESE_INFO("CONNECTION_STRING: %s", SQLITE_CONNECTION_STRING);
    return RUN_ALL_TESTS();
}