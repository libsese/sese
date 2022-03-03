#include "Util.h"
#include "util/DateTime.h"

int main() {
    auto start = sese::DateTime::now(8);
    auto dateTime1 = sese::DateTime(1646265600);// 2022-03-03 00:00:00
    auto dateTime2 = sese::DateTime(1646092800);// 2022-03-01 00:00:00
    auto timespan = dateTime1 - dateTime2;
    ASSERT(timespan.getDays() == 2)

    auto rt1 = dateTime1.compareTo(dateTime2);
    ASSERT(rt1 == 1);

    auto rt2 = dateTime2.compareTo(dateTime1);
    ASSERT(rt2 == -1);

    auto end = sese::DateTime::now(8);
    auto rt3 = start->unclearCompareTo(*end);
    ASSERT(rt3 == 0)

    return 0;
}