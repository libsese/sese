#include "sese/convert/MessageDigest.h"
#include "sese/security/MessageDigest.h"
#include "sese/io/InputBufferWrapper.h"
#include "sese/io/FileStream.h"
#include "gtest/gtest.h"

using sese::security::MessageDigest;

// TEST(TestMessageDigest, MD4) {
//     const char *str0 = "Hello";
//     const char *rt0 = "a58fc871f5f68e4146474ac1e2f07419";
//     {
//         auto input = sese::InputBufferWrapper(str0, 5);
//         auto rt = MessageDigest::digest(MessageDigest::Type::MD4, &input);
//         EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
//     }
// }

TEST(TestMessageDigest, MD5) {
    const char *str0 = "Hello";
    const char *rt0 = "8b1a9953c4611296a827abf8c47804d7";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "\x34\x35\x22\x23\xAF\x44\xE4";
    const char *rt1 = "d331385d2ce8241759a328d111f82894";
    {
        auto input = sese::io::InputBufferWrapper(str1, 7);
        auto rt = MessageDigest::digest(MessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

TEST(TestMessageDigest, SHA1) {
    const char *str0 = "Hello";
    const char *rt0 = "f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "Hello, World";
    const char *rt1 = "907d14fb3af2b0d4f18c2d46abe8aedce17367bd";
    {
        auto input = sese::io::InputBufferWrapper(str1, 12);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

// TEST(TestMessageDigest, SHA224) {
//     const char *str0 = "Hello";
//     const char *rt0 = "4149da18aa8bfc2b1e382c6c26556d01a92c261b6436dad5e3be3fcc";
//     {
//         auto input = sese::InputBufferWrapper(str0, 5);
//         auto rt = MessageDigest::digest(MessageDigest::Type::SHA224, &input);
//         EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
//     }
// }

TEST(TestMessageDigest, SHA256) {
    const char *str0 = "Hello";
    const char *rt0 = "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA256, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, SHA384) {
    const char *str0 = "Hello";
    const char *rt0 = "3519fe5ad2c596efe3e276a6f351b8fc0b03db861782490d45f7598ebd0ab5fd5520ed102f38c4a5ec834e98668035fc";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA384, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, SHA512) {
    const char *str0 = "Hello";
    const char *rt0 = "3615f80c9d293ed7402687f94b22d58e529b8cc7916f8fac7fddf7fbd5af4cf777d3d795a7a00a16bf7e7f3fb9561ee9baae480da9fe7a18769e71886b03f315";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA512, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

using OldMessageDigest = sese::MessageDigest;

TEST(TestOldMessageDigest, MD5) {
    const char *str0 = "Hello";
    const char *rt0 = "8b1a9953c4611296a827abf8c47804d7";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = OldMessageDigest::digest(OldMessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "\x34\x35\x22\x23\xAF\x44\xE4";
    const char *rt1 = "d331385d2ce8241759a328d111f82894";
    {
        auto input = sese::io::InputBufferWrapper(str1, 7);
        auto rt = OldMessageDigest::digest(OldMessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

TEST(TestOldMessageDigest, MD5File_0) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/gtest/Data/checksum-size-112.txt", "rb");
    ASSERT_NE(file, nullptr);

    const char *expect = "05055d054939c7b713f9fefec599daa5";
    auto rt = OldMessageDigest::digest(OldMessageDigest::MD5, file);
    EXPECT_TRUE(strcmp(rt.get(), expect) == 0);
}

TEST(TestOldMessageDigest, MD5File_1) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/gtest/Data/checksum-size-122.txt", "rb");
    ASSERT_NE(file, nullptr);

    const char *expect = "c13edc8b492731630ad0a983ef66f292";
    auto rt = OldMessageDigest::digest(OldMessageDigest::MD5, file);
    EXPECT_TRUE(strcmp(rt.get(), expect) == 0);
}

TEST(TestOldMessageDigest, MD5File_2) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/gtest/Data/checksum-size-128.txt", "rb");
    ASSERT_NE(file, nullptr);

    const char *expect = "382b83364eaaebb4770851d9b704457b";
    auto rt = OldMessageDigest::digest(OldMessageDigest::MD5, file);
    EXPECT_TRUE(strcmp(rt.get(), expect) == 0);
}

TEST(TestOldMessageDigest, SHA1File) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/gtest/Data/checksum-size-122.txt", "rb");
    ASSERT_NE(file, nullptr);

    const char *expect = "632baa1212f10189290a79dd109219d501d36aec";
    auto rt = OldMessageDigest::digest(OldMessageDigest::SHA1, file);
    EXPECT_TRUE(strcmp(rt.get(), expect) == 0);
}

TEST(TestOldMessageDigest, SHA256File) {
    auto file = sese::io::FileStream::create(PROJECT_PATH "/gtest/Data/checksum-size-122.txt", "rb");
    ASSERT_NE(file, nullptr);

    const char *expect = "82bbdd6fdd8a848f703ad941ca491938ada0d98c97b45a76162436b04f95a840";
    auto rt = OldMessageDigest::digest(OldMessageDigest::SHA256, file);
    EXPECT_TRUE(strcmp(rt.get(), expect) == 0);
}

TEST(TestOldMessageDigest, SHA1) {
    const char *str0 = "Hello";
    const char *rt0 = "f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = OldMessageDigest::digest(OldMessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "Hello, World";
    const char *rt1 = "907d14fb3af2b0d4f18c2d46abe8aedce17367bd";
    {
        auto input = sese::io::InputBufferWrapper(str1, 12);
        auto rt = OldMessageDigest::digest(OldMessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

TEST(TestOldMessageDigest, SHA256) {
    const char *str0 = "Hello";
    const char *rt0 = "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969";
    {
        auto input = sese::io::InputBufferWrapper(str0, 5);
        auto rt = OldMessageDigest::digest(OldMessageDigest::Type::SHA256, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}