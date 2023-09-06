#include <sese/util/Util.h>

#include <map>

#include <gtest/gtest.h>

TEST(TestUtil, Find) {
    const char *str = "1234567890";
    auto i = sese::findFirstAt(str, '3');
    ASSERT_EQ(i, 2);

    auto j = sese::findFirstAt(str, 'a');
    ASSERT_EQ(j, -1);
}

TEST(TestUtil, Operator) {
    auto i = sese::StrCmpI()("ABC", "abc");
    ASSERT_EQ(i, 0);

    auto j = sese::StrCmp()("ABC", "abc");
    ASSERT_NE(j, 0);
}

TEST(TestUtil, Misc) {
    puts(::getSpecificVersion());

    auto err = sese::getErrorCode();
    auto str = sese::getErrorString(err);
    puts(str.c_str());
}

#include <sese/util/Test.h>

TEST(TestUtil, Backtrace) {
    auto backtrace = sese::Test::backtrace2String(2, "");
    puts(backtrace.c_str());
}

#include <sese/util/OutputBufferWrapper.h>
#include <sese/util/OutputUtil.h>
#include <sese/text/String.h>

TEST(TestUtil, OutputUtil) {
    char buf0;
    auto output0 = sese::OutputBufferWrapper(&buf0, 1);
    output0 << 'A';
    EXPECT_EQ(buf0, 'A');

    char buf1[16]{};
    auto output1 = sese::OutputBufferWrapper(buf1, sizeof(buf1));
    auto str = sese::text::String::fromUTF8("你好");
    output1 << str;
    EXPECT_EQ(str, sese::text::StringView(buf1));

    char buf2[16]{};
    auto output2 = sese::OutputBufferWrapper(buf2, sizeof(buf2));
    auto view = sese::text::StringView("こんにちは");
    output2 << view;
    EXPECT_EQ(view, sese::text::StringView(buf2));
}

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::Initializer::getInitializer();
    return RUN_ALL_TESTS();
}