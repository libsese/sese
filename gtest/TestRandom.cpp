#include "sese/config/CSVWriter.h"
#include "sese/io/FileStream.h"
#include "sese/util/Random.h"
#include "gtest/gtest.h"

#include <random>

TEST(TestRandom, STD) {
    sese::Random device;
    ASSERT_EQ(device.entropy(), 64);
    std::uniform_int_distribution<int> distribution(-150000, 150000);

    auto file = sese::io::FileStream::create("random.csv", "wt");
    ASSERT_NE(file, nullptr);
    auto writer = sese::CSVWriter(file.get(), ',', false);
    for (int i = 0; i < 300; i++) {
        std::default_random_engine engine((unsigned int) device());
        writer.write({std::to_string(i), std::to_string(distribution(engine))});
    }
    file->close();
}

TEST(TestRandom, Misc) {
    ASSERT_EQ(sese::Random::max(), UINT64_MAX);
    ASSERT_EQ(sese::Random::min(), 0);
}