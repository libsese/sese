#include "sese/config/CSVWriter.h"
#include "sese/util/FileStream.h"
#include "sese/util/Random.h"
#include "gtest/gtest.h"

#include <random>

TEST(TestRandom, STD) {
    sese::Random device;
    std::uniform_int_distribution<int> distribution(-150000, 150000);

    auto file = sese::FileStream::create("random.csv", "wt");
    ASSERT_NE(file , nullptr);
    auto writer = sese::CSVWriter(file.get(), ',', false);
    for (int i = 0 ; i < 300000; i++) {
        std::default_random_engine engine(device());
        writer.write({std::to_string(i), std::to_string(distribution(engine))});
    }
    file->close();
}