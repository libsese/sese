#include <gtest/gtest.h>

#include <sese/util/MemoryViewer.h>
#include <sese/util/ConsoleOutputStream.h>
#include <sese/util/OutputUtil.h>

TEST(TestMemoryViewer, _0) {
    int8_t i8 = 20;
    int16_t i16 = 40;
    int32_t i32 = 90;
    int64_t i64 = 120;

    sese::ConsoleOutputStream output;

    sese::MemoryViewer::peer8(&output, &i8, true);
    output << "\n";
    sese::MemoryViewer::peer16(&output, &i16, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64, true);
    output << "\n";

    sese::MemoryViewer::peer16(&output, &i16,EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer16(&output, &i16,EndianType::Little, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32,EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32,EndianType::Little, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64,EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64,EndianType::Little, true);
    output << "\n";
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}