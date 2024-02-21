#include <gtest/gtest.h>

#include <sese/config/UniReader.h>
#include <sese/record/Marco.h>

TEST(TestUniReader, Open_0) {
    auto reader = sese::UniReader::create(PROJECT_PATH "/test/Data/data-0.txt");
    GTEST_ASSERT_NE(reader, nullptr);

    std::string str;
    while (!(str = reader->readLine()).empty()) {
        SESE_INFO("%s", str.c_str());
    }
}

TEST(TestUniReader, Open_1) {
    GTEST_ASSERT_EQ(sese::UniReader::create("undef.txt"), nullptr);
}