// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sese/convert/Decompressor.h"
#include "sese/convert/Compressor.h"
#include "sese/convert/GZipFileOutputStream.h"
#include "sese/convert/GZipFileInputStream.h"
#include "sese/record/LogHelper.h"
#include "sese/io/ByteBuilder.h"
#include "sese/io/OutputBufferWrapper.h"

#include "gtest/gtest.h"

#include <filesystem>

TEST(TestCompress, ZLIB) {
    sese::Compressor compressor(sese::CompressionType::ZLIB, 9, 8);
    sese::io::ByteBuilder builder(512);

    char temp[8];
    sese::io::OutputBufferWrapper out(temp, 8);

    char compress_buffer_in[] = {"using zlib with cpp"};
    compressor.input(compress_buffer_in, sizeof(compress_buffer_in));

    int rt;
    size_t last_time = 0;
    do {
        rt = compressor.deflate(&out);
        sese::record::LogHelper::d("compressing (%d/%d)", compressor.getTotalIn(), sizeof(compress_buffer_in));
        auto current = compressor.getTotalOut() - last_time;
        last_time = compressor.getTotalOut();
        builder.write(temp, current);
        out.reset();
    } while (rt != 0);
    sese::record::LogHelper::d("compress done");

    char compress_buffer_out[32]{};
    auto compress_size = builder.read(compress_buffer_out, 32);
    builder.freeCapacity();
    sese::Decompressor decompressor(sese::CompressionType::ZLIB, 8);
    decompressor.input(compress_buffer_out, static_cast<unsigned int>(compress_size));
    last_time = 0;
    do {
        rt = decompressor.inflate(&out);
        sese::record::LogHelper::d("decompressing (%d/%d)", decompressor.getTotalIn(), compress_size);
        auto current = decompressor.getTotalOut() - last_time;
        last_time = decompressor.getTotalOut();
        builder.write(temp, current);
        out.reset();
    } while (rt != 0);
    sese::record::LogHelper::d("decompress done");

    char decompress_buffer_out[32]{};
    builder.read(decompress_buffer_out, 32);

    ASSERT_TRUE(strcmp(decompress_buffer_out, compress_buffer_in) == 0);
}

TEST(TestCompress, GZIP) {
    using sese::GZipFileInputStream;
    using sese::GZipFileOutputStream;

    auto output = GZipFileOutputStream::create("test.txt.gz", 10); // 10 will replace to 9
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

    std::filesystem::remove("test.txt.gz");

    ASSERT_EQ(GZipFileOutputStream::create("./undef/undef.gz", 9), nullptr);
    ASSERT_EQ(GZipFileInputStream::create("./undef/undef.gz"), nullptr);
}