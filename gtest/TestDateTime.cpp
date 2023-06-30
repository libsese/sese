#include "sese/util/DateTime.h"
#include "sese/record/LogHelper.h"
#include "sese/text/DateTimeFormatter.h"
#include "gtest/gtest.h"

TEST(TestDateTime, Info) {
    sese::record::LogHelper log("Info");
    auto time = sese::DateTime::now(8);
    log.info("is leap year: %d", time->isLeapYear());
    log.info("%" PRId64, time->getTimestamp());
    log.info("%" PRId64, time->getUSecond());
}

TEST(TestDateTime, Compare) {
    // 2022-03-03 00:00:00
    auto time1 = sese::DateTime(1646265600);
    // 2022-03-01 00:00:00
    auto time2 = sese::DateTime(1646092800);

    auto span = time1 - time2;
    EXPECT_TRUE(span.getDays() == 2);

    EXPECT_TRUE(time1.compareTo(time2) == 1);
    EXPECT_TRUE(time2.compareTo(time1) == -1);
}

TEST(TestDateTime, UnclearCompare) {
    auto time1 = sese::DateTime::now(8);
    auto time2 = sese::DateTime::now(8);
    EXPECT_TRUE(time1->unclearCompareTo(*time2) == 0);
}

TEST(TestDateTime, Formatter) {
    sese::record::LogHelper log("Formatter");

    auto time1 = sese::DateTime::now();
    auto str1 = sese::text::DateTimeFormatter::format(time1, "%D%a%t%e ddd dddd UTCz yyyy-MM-dd HH:mm:ss.ff.ffff");
    EXPECT_TRUE(!str1.empty());
    log.info(str1.c_str());
    auto time2 = sese::DateTime::now(0);
    auto str2 = sese::text::DateTimeFormatter::format(time2, TIME_GREENWICH_MEAN_PATTERN);
    EXPECT_TRUE(!str2.empty());
    log.info(str2.c_str());
}

TEST(TestDateTime, Parse) {
    using sese::text::DateTimeFormatter;

    EXPECT_EQ(
            DateTimeFormatter::parseFromGreenwich("Tue, 17 Oct 2023 15:41:22 GMT"),
            1697557282
    );

    EXPECT_EQ(
            DateTimeFormatter::parseFromISO8601("2023-06-20"),
            1687219200
    );

    EXPECT_EQ(
            DateTimeFormatter::parseFromISO8601("2023-06-20 16:46:55"),
            1687279615
    );

    EXPECT_EQ(
            DateTimeFormatter::parseFromISO8601("2023-06-20T16:46:55Z"),
            1687279615
    );

    EXPECT_EQ(
            DateTimeFormatter::parseFromISO8601("2023-06-20 16:46:55 +8"),
            1687250815
    );

    EXPECT_EQ(
            DateTimeFormatter::parseFromISO8601("2023-06-20T16:46:55+00:00"),
            1687279615
    );
}