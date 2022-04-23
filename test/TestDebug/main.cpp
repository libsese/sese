#include <sese/Test.h>
#include <sese/record/LogHelper.h>

using sese::Test;
using sese::LogHelper;

LogHelper helper("fDEBUG"); // NOLINT

int main() {
    Test::assert(helper, false, 0);
    Test::assert(helper, false, 0);
    Test::assert(helper, true, 0);
    Test::assert(helper, false, 0);
    Test::assert(helper, false, 0);
    Test::assert(helper, false, 0);
    return 0;
}