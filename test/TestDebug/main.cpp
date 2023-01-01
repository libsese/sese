#include "sese/util/Test.h"
#include <sese/record/LogHelper.h>

using sese::record::LogHelper;

LogHelper helper("fDEBUG"); // NOLINT

int main() {
    assert(helper, false, 0);
    assert(helper, false, 0);
    assert(helper, true, 0);
    assert(helper, false, 0);
    assert(helper, false, 0);
    assert(helper, false, 0);
    assert(helper, false == true, 0);
    return 0;
}