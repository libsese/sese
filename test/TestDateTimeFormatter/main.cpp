#include "DateTimeFormatter.h"

int main() {
    auto time1 = sese::DateTime::now();
    auto rt1 = sese::DateTimeFormatter::format(time1, "%D%a%t%e ddd dddd UTCz yyyy-MM-dd HH:mm:ss.ff.ffff");
    puts(rt1.c_str());
    auto time2 = sese::DateTime::now(0);
    auto rt2 = sese::DateTimeFormatter::format(time2, GREENWICH_MEAN_TIME_PATTERN);
    puts(rt2.c_str());
    return 0;
}