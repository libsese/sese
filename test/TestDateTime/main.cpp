#include "sese/util/DateTime.h"
#include "sese/record/LogHelper.h"

sese::record::LogHelper helper("fDATETIME"); // NOLINT

int main() {
    auto start = sese::DateTime::now(8);
    helper.info("is leap year: %d", start->isLeapYear());
    helper.info("%" PRId64, start->getTimestamp());
    helper.info("%" PRId64, start->getUSecond());
    auto dateTime1 = std::make_shared<sese::DateTime>(1646265600);// 2022-03-03 00:00:00
    auto dateTime2 = std::make_shared<sese::DateTime>(1646092800);// 2022-03-01 00:00:00
    auto timeSpan = *dateTime1 - *dateTime2;
    if (timeSpan.getDays() != 2) return -1;

    auto rt1 = dateTime1->compareTo(*dateTime2);
    if (rt1 != 1) return -1;

    auto rt2 = dateTime2->compareTo(*dateTime1);
    if (rt2 != -1) return -1;

    auto end = sese::DateTime::now(8);
    auto rt3 = start->unclearCompareTo(*end);
    if (rt3 != 0) return -1;

    return 0;
}