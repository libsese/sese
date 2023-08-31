#include <sese/util/UuidBuilder.h>

#include <gtest/gtest.h>

TEST(TestUuidBuilder, _0) {
    auto builder = sese::UuidBuilder();
    uuid::Uuid id;
    ASSERT_TRUE(builder.generate(id));
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}