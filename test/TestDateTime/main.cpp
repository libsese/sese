#include "Util.h"
#include "DateTime.h"

#define FILTER_TEST_DATETIME "fDATETIME"

int main() {
    auto start = sese::DateTime::now(8);
    ROOT_INFO(FILTER_TEST_DATETIME, "is leap year: %d", start->isLeapYear())
    ROOT_INFO(FILTER_TEST_DATETIME, "%" PRId64, start->getTimestamp())
    ROOT_INFO(FILTER_TEST_DATETIME, "%" PRId64, start->getUSecond())
    auto dateTime1 = std::make_shared<sese::DateTime>(1646265600);// 2022-03-03 00:00:00
    auto dateTime2 = std::make_shared<sese::DateTime>(1646092800);// 2022-03-01 00:00:00
    auto timespan = *dateTime1 - *dateTime2;
    ASSERT(FILTER_TEST_DATETIME, timespan.getDays() == 2)

    auto rt1 = dateTime1->compareTo(*dateTime2);
    ASSERT(FILTER_TEST_DATETIME, rt1 == 1)

    auto rt2 = dateTime2->compareTo(*dateTime1);
    ASSERT(FILTER_TEST_DATETIME, rt2 == -1)

    auto end = sese::DateTime::now(8);
    auto rt3 = start->unclearCompareTo(*end);
    ASSERT(FILTER_TEST_DATETIME, rt3 == 0)

    return 0;
}