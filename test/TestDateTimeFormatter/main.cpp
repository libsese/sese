#include "util/DateTimeFormatter.h"

int main() {
    auto time = sese::DateTime::now();
    auto rt = sese::DateTimeFormatter::format(time, "%D%a%t%e UTCz yyyy-MM-dd HH:mm:ss.ff.ffff");
    puts(rt.c_str());
    return 0;
}