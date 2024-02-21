#include <gtest/gtest.h>

#include <sese/util/MemoryViewer.h>
#include "sese/io/ConsoleOutputStream.h"
#include "sese/io/OutputUtil.h"

TEST(TestMemoryViewer, _0) {
    int8_t i8 = 20;
    int16_t i16 = 40;
    int32_t i32 = 90;
    int64_t i64 = 120;

    sese::io::ConsoleOutputStream output;

    sese::MemoryViewer::peer8(&output, &i8, true);
    output << "\n";
    sese::MemoryViewer::peer16(&output, &i16, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64, true);
    output << "\n";

    sese::MemoryViewer::peer16(&output, &i16, EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer16(&output, &i16, EndianType::Little, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32, EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer32(&output, &i32, EndianType::Little, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64, EndianType::Big, true);
    output << "\n";
    sese::MemoryViewer::peer64(&output, &i64, EndianType::Little, true);
    output << "\n";
}