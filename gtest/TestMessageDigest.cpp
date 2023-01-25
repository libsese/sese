#include "sese/security/MessageDigest.h"
#include "sese/util/InputBufferWrapper.h"
#include "gtest/gtest.h"

using sese::security::MessageDigest;

TEST(TestMessageDigest, MD4) {
    const char *str0 = "Hello";
    const char *rt0 = "a58fc871f5f68e4146474ac1e2f07419";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::MD4, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, MD5) {
    const char *str0 = "Hello";
    const char *rt0 = "8b1a9953c4611296a827abf8c47804d7";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "\x34\x35\x22\x23\xAF\x44\xE4";
    const char *rt1 = "d331385d2ce8241759a328d111f82894";
    {
        auto input = sese::InputBufferWrapper(str1, 7);
        auto rt = MessageDigest::digest(MessageDigest::Type::MD5, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

TEST(TestMessageDigest, SHA1) {
    const char *str0 = "Hello";
    const char *rt0 = "f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }

    const char *str1 = "Hello, World";
    const char *rt1 = "907d14fb3af2b0d4f18c2d46abe8aedce17367bd";
    {
        auto input = sese::InputBufferWrapper(str1, 12);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA1, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt1) == 0);
    }
}

TEST(TestMessageDigest, SHA224) {
    const char *str0 = "Hello";
    const char *rt0 = "4149da18aa8bfc2b1e382c6c26556d01a92c261b6436dad5e3be3fcc";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA224, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, SHA256) {
    const char *str0 = "Hello";
    const char *rt0 = "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA256, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, SHA384) {
    const char *str0 = "Hello";
    const char *rt0 = "3519fe5ad2c596efe3e276a6f351b8fc0b03db861782490d45f7598ebd0ab5fd5520ed102f38c4a5ec834e98668035fc";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA384, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}

TEST(TestMessageDigest, SHA512) {
    const char *str0 = "Hello";
    const char *rt0 = "3615f80c9d293ed7402687f94b22d58e529b8cc7916f8fac7fddf7fbd5af4cf777d3d795a7a00a16bf7e7f3fb9561ee9baae480da9fe7a18769e71886b03f315";
    {
        auto input = sese::InputBufferWrapper(str0, 5);
        auto rt = MessageDigest::digest(MessageDigest::Type::SHA512, &input);
        EXPECT_TRUE(strcmp(rt.get(), rt0) == 0);
    }
}