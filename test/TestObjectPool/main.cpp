#include <sese/concurrent/ConcurrentObjectPool.h>
#include "sese/util/ObjectPool.h"
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"
#include <sese/thread/ThreadPool.h>
#include "sese/util/Util.h"

using sese::record::LogHelper;
using sese::ObjectPool;
using sese::Test;
using sese::ThreadPool;
using sese::concurrent::ConcurrentObjectPool;

sese::record::LogHelper helper("fOBJECT_POOL");// NOLINT

void testRecycle() {
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
}

void testThreadSafety() {
    ThreadPool threadPool("POOL", 4);
    ObjectPool<int32_t>::Ptr objectPool = ObjectPool<int32_t>::create();

    for (int i = 0; i < 1000; i++) {
        threadPool.postTask([&objectPool]() {
            ObjectPool<int32_t>::ObjectPtr object = objectPool->borrow();
            *object += 1;
        });
    }

    sese::sleep(3);
    threadPool.shutdown();
}

// void testConcurrent() {
//     ThreadPool threadPool("POOL", 8);
//     ConcurrentObjectPool<int32_t>::Ptr objectPool = ConcurrentObjectPool<int32_t>::create();

//     for (int i = 0; i < 500; i++) {
//         threadPool.postTask([&objectPool]() {
//             ObjectPool<int32_t>::ObjectPtr object = objectPool->borrow();
//             *object += 1;
//         });
//     }

//     sese::sleep(3);
//     threadPool.shutdown();
// }

int main() {
    testRecycle();
    testThreadSafety();
    // testConcurrent();
    return 0;
}