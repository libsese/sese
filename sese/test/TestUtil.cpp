#include <sese/util/Util.h>

#include <map>

#include <gtest/gtest.h>

TEST(TestUtil, Find) {
    const char *str = "1234567890";
    auto i = sese::findFirstAt(str, '3');
    EXPECT_EQ(i, 2);

    auto j = sese::findFirstAt(str, 'a');
    EXPECT_EQ(j, -1);
}

TEST(TestUtil, Operator) {
    auto i = sese::StrCmpI()("ABC", "abc");
    EXPECT_EQ(i, 0);

    auto j = sese::StrCmp()("ABC", "abc");
    EXPECT_NE(j, 0);
}

TEST(TestUtil, Number2StringLength) {
    EXPECT_EQ(sese::number2StringLength(-1, 10), 2);
    EXPECT_EQ(sese::number2StringLength(0, 10), 1);
    EXPECT_EQ(sese::number2StringLength(65535, 10), 5);
    EXPECT_EQ(sese::number2StringLength(-128, 10), 4);

    EXPECT_EQ(sese::number2StringLength(129, 16), 2);
    EXPECT_EQ(sese::number2StringLength(18794, 8), 5);
}

TEST(TestUtil, Misc) {
    puts(::getSpecificVersion());

    auto err = sese::getErrorCode();
    auto str = sese::getErrorString(err);
    puts(str.c_str());
}

#include <sese/io/OutputBufferWrapper.h>
#include <sese/io/OutputUtil.h>
#include <sese/text/String.h>

TEST(TestUtil, OutputUtil) {
    char buf0;
    auto output0 = sese::io::OutputBufferWrapper(&buf0, 1);
    output0 << 'A';
    EXPECT_EQ(buf0, 'A');

    char buf1[16]{};
    auto output1 = sese::io::OutputBufferWrapper(buf1, sizeof(buf1));
    auto str = sese::text::String::fromUTF8("你好");
    output1 << str;
    EXPECT_EQ(str, sese::text::StringView(buf1));

    char buf2[16]{};
    auto output2 = sese::io::OutputBufferWrapper(buf2, sizeof(buf2));
    auto view = sese::text::StringView("こんにちは");
    output2 << view;
    EXPECT_EQ(view, sese::text::StringView(buf2));
}