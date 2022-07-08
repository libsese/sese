#include <sese/ObjectPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::LogHelper;
using sese::ObjectPool;
using sese::Test;

LogHelper helper("fOBJECT_POOL");// NOLINT

int main() {
    ObjectPool<int>::ObjectPtr i0;
    {
        auto pool = ObjectPool<int>::create();
        // i0 生命周期大于 pool
        i0 = pool->borrow();
        *i0 = 255;
        {
            auto i1 = pool->borrow();
            *i1 = 10;
        }
        {
            auto i2 = pool->borrow();
            assert(helper, *i2 == 10, -1)
        }
    }
    assert(helper, *i0 == 255, -2)
    return 0;
}