#include <sese/util/Version.h>

#include <gtest/gtest.h>

using sese::Version;

TEST(TestVersion, TestConstructor) {
    Version version(1, 2, 3, 4);
    EXPECT_EQ(version.getMajor(), 1);
    EXPECT_EQ(version.getMinor(), 2);
    EXPECT_EQ(version.getPatch(), 3);
    EXPECT_EQ(version.getRevision(), 4);
}

TEST(TestVersion, TestParse) {
    {
        auto option = Version::parse("2");
        ASSERT_TRUE(option.has_value());
        auto version = option.value();
        EXPECT_EQ(version.getMajor(), 2);
        EXPECT_EQ(version.getMinor(), 0);
    }
    {
        auto option = Version::parse("1A");
        ASSERT_FALSE(option.has_value());
    }
    {
        auto option = Version::parse("1.2");
        ASSERT_TRUE(option.has_value());
        auto version = option.value();
        EXPECT_EQ(version.getMajor(), 1);
        EXPECT_EQ(version.getMinor(), 2);
        EXPECT_EQ(version.getPatch(), 0);
    }
    {
        auto option = Version::parse("1.2A");
        ASSERT_FALSE(option.has_value());
    }
    {
        auto option = Version::parse("1.2.3");
        ASSERT_TRUE(option.has_value());
        auto version = option.value();
        EXPECT_EQ(version.getMajor(), 1);
        EXPECT_EQ(version.getMinor(), 2);
        EXPECT_EQ(version.getPatch(), 3);
        EXPECT_EQ(version.getRevision(), 0);
    }
    {
        auto option = Version::parse("1.2.3A");
        ASSERT_FALSE(option.has_value());
    }
    {
        auto option = Version::parse("1.2.3.4");
        ASSERT_TRUE(option.has_value());
        auto version = option.value();
        EXPECT_EQ(version.getMajor(), 1);
        EXPECT_EQ(version.getMinor(), 2);
        EXPECT_EQ(version.getPatch(), 3);
        EXPECT_EQ(version.getRevision(), 4);
    }
    {
        auto option = Version::parse("1.2.3.4A");
        ASSERT_FALSE(option.has_value());
    }
}

TEST(TestVersion, TestString) {
    Version version(1, 2, 3, 4);
    EXPECT_EQ(version.toString(), "1.2.3.4");
}

TEST(TestVersion, TestShortString) {
    Version version0(1, 2, 3, 4);
    EXPECT_EQ(version0.toShortString(), "1.2.3.4");

    Version version1(1, 2, 3, 0);
    EXPECT_EQ(version1.toShortString(), "1.2.3");

    Version version2(1, 2, 0, 0);
    EXPECT_EQ(version2.toShortString(), "1.2");

    Version version3(1, 0, 0, 0);
    EXPECT_EQ(version3.toShortString(), "1");

    Version version4(0, 0, 0, 0);
    EXPECT_EQ(version4.toShortString(), "0");

    Version version5(1, 0, 3, 0);
    EXPECT_EQ(version5.toShortString(), "1.0.3");
}

TEST(TestVersion, TestEquality) {
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 1, 1, 1);
        EXPECT_TRUE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 1, 1, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 1, 0, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 1, 0, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 0, 1, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 0, 1, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 0, 0, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(1, 0, 0, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 1, 1, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 1, 0, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 1, 0, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 0, 1, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 0, 1, 0);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 0, 0, 1);
        EXPECT_FALSE(version1 == version2);
    }
    {
        Version version1(1, 1, 1, 1);
        Version version2(0, 0, 0, 0);
        EXPECT_FALSE(version1 == version2);
    }
}

TEST(TestVersion, TestInequality) {
    Version version1(1, 1, 1, 1);
    Version version2(1, 1, 1, 1);
    EXPECT_FALSE(version1 != version2);

    Version version3(1, 1, 1, 0);
    Version version4(1, 1, 1, 1);
    EXPECT_TRUE(version3 != version4);
}

TEST(TestVersion, TestGreater) {
    Version version1(1, 2, 3, 4);
    Version version2(1, 2, 3, 3);
    EXPECT_TRUE(version1 > version2);
}

TEST(TestVersion, TestLess) {
    Version version1(1, 2, 3, 4);
    Version version2(1, 2, 3, 5);
    EXPECT_TRUE(version1 < version2);
}

TEST(TestVersion, TestGreaterOrEqual) {
    Version version1(1, 2, 3, 4);
    Version version2(1, 2, 3, 4);
    EXPECT_TRUE(version1 >= version2);

    Version version3(1, 2, 3, 4);
    Version version4(1, 2, 3, 3);
    EXPECT_TRUE(version3 >= version4);
}

TEST(TestVersion, TestLessOrEqual) {
    Version version1(1, 2, 3, 4);
    Version version2(1, 2, 3, 4);
    EXPECT_TRUE(version1 <= version2);

    Version version3(1, 2, 3, 4);
    Version version4(1, 2, 3, 5);
    EXPECT_TRUE(version3 <= version4);
}