#include "sese/convert/Decompressor.h"
#include "sese/convert/Compressor.h"
#include "sese/record/LogHelper.h"
#include "sese/util/ByteBuilder.h"
#include "sese/util/OutputBufferWrapper.h"
#include "gtest/gtest.h"

TEST(TestCompress, ZLIB) {
    sese::Compressor compressor(sese::CompressionType::ZLIB, 9, 8);
    sese::ByteBuilder compressBuilder(512);

    char temp[8];
    sese::OutputBufferWrapper out(temp, 8);

    char compressBufferIn[] = {"using zlib with cpp"};
    compressor.input(compressBufferIn, sizeof(compressBufferIn) - 1);

    int rt;
    size_t lastTime = 0;
    do {
        rt = compressor.deflate(&out);
        sese::record::LogHelper::d("compressing (%d/%d)", compressor.getTotalIn(), sizeof(compressBufferIn));
        auto current = compressor.getTotalOut() - lastTime;
        lastTime = compressor.getTotalOut();
        compressBuilder.write(temp, current);
        out.reset();
    } while (rt != 0);
    sese::record::LogHelper::d("compress done");
}