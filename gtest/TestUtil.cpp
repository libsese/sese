#include <sese/util/Util.h>
#include <map>

#include <gtest/gtest.h>

TEST(TestUtil, Find){
    const char *str = "1234567890";
    auto i = sese::findFirstAt(str, '3');
    ASSERT_EQ(i, 2);

    auto j = sese::findFirstAt(str, 'a');
    ASSERT_EQ(j, -1);
}

TEST(TestUtil, Operator) {
    auto i = sese::StrCmpI()("ABC","abc");
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