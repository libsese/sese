#include "sese/convert/Decompressor.h"
#include "sese/convert/Compressor.h"
#include "sese/record/LogHelper.h"
#include "sese/util/ByteBuilder.h"
#include "sese/util/OutputBufferWrapper.h"
#include "gtest/gtest.h"

TEST(TestCompress, ZLIB) {
    sese::Compressor compressor(sese::CompressionType::ZLIB, 9, 8);
    sese::ByteBuilder builder(512);

    char temp[8];
    sese::OutputBufferWrapper out(temp, 8);

    char compressBufferIn[] = {"using zlib with cpp"};
    compressor.input(compressBufferIn, sizeof(compressBufferIn));

    int rt;
    size_t lastTime = 0;
    do {
        rt = compressor.deflate(&out);
        sese::record::LogHelper::d("compressing (%d/%d)", compressor.getTotalIn(), sizeof(compressBufferIn));
        auto current = compressor.getTotalOut() - lastTime;
        lastTime = compressor.getTotalOut();
        builder.write(temp, current);
        out.reset();
    } while (rt != 0);
    sese::record::LogHelper::d("compress done");

    char compressBufferOut[32]{};
    auto compressSize = builder.read(compressBufferOut, 32);
    builder.freeCapacity();
    sese::Decompressor decompressor(sese::CompressionType::ZLIB, 8);
    decompressor.input(compressBufferOut, (unsigned int) compressSize);
    lastTime = 0;
    do {
        rt = decompressor.inflate(&out);
        sese::record::LogHelper::d("decompressing (%d/%d)", decompressor.getTotalIn(), compressSize);
        auto current = decompressor.getTotalOut() - lastTime;
        lastTime = decompressor.getTotalOut();
        builder.write(temp, current);
        out.reset();
    } while (rt != 0);
    sese::record::LogHelper::d("decompress done");

    char decompressBufferOut[32]{};
    builder.read(decompressBufferOut, 32);

    ASSERT_TRUE(strcmp(decompressBufferOut, compressBufferIn) == 0);
}