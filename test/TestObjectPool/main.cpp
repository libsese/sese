#include <sese/ObjectPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::ObjectPool;
using sese::LogHelper;
using sese::Test;

LogHelper helper("fOBJECT_POOL");// NOLINT

int main() {
    sese::ObjectPool<int32_t> pool;
    auto borrowObject1 = pool.borrowObject();
    *borrowObject1 = 10;
    auto borrowObject2 = pool.borrowObject();
    *borrowObject2 = 20;
    pool.returnObject(borrowObject1);
    auto borrowObject3 = pool.borrowObject();
    Test::assert(helper, *borrowObject3 == 10);
    pool.returnObject(borrowObject3);

    return 0;
}