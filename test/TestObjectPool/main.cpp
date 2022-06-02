#include <sese/ObjectPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::LogHelper;
using sese::Recyclable;
using sese::Recycler;
using sese::Test;

LogHelper helper("fOBJECT_POOL");// NOLINT

int main() {
    Recycler<int> recycler(1);
    auto value1 = recycler.get();
    auto value2 = recycler.get();
    auto value3 = recycler.get();

    recycler.recycle(value1);
    recycler.recycle(value2);

    return 0;
}