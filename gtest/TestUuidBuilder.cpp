#include <sese/util/UuidBuilder.h>

#include <gtest/gtest.h>

TEST(TestUuidBuilder, _0) {
    auto builder = sese::UuidBuilder();
    uuid::Uuid id;
    ASSERT_TRUE(builder.generate(id));
}