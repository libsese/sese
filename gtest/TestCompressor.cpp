#include "sese/convert/Decompressor.h"
#include "sese/convert/Compressor.h"
#include "sese/convert/GZipFileOutputStream.h"
#include "sese/convert/GZipFileInputStream.h"
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

TEST(TestCompress, GZIP) {
    using sese::GZipFileInputStream;
    using sese::GZipFileOutputStream;

    auto output = GZipFileOutputStream::create("test.txt.gz", 9);
    ASSERT_TRUE(output);
    for (int i = 0; i < 10; ++i) {
        output->write("Hello, World\n", 13);
    }
    output->close();

    auto input = GZipFileInputStream::create("test.txt.gz");
    ASSERT_TRUE(input);
    char buffer[1024]{};
    input->read(buffer, 1024);
    input->close();
    puts(buffer);

    ASSERT_EQ(GZipFileOutputStream::create("./undef/undef.gz", 9), nullptr);
    ASSERT_EQ(GZipFileInputStream::create("./undef/undef.gz"), nullptr);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}