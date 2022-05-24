#include <sese/ObjectPool.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

using sese::Recyclable;
using sese::Recycler;
using sese::LogHelper;
using sese::Test;

class Data : public Recyclable {
public:
    Data() : Recyclable() {}

    void recycle() noexcept override {
        value = 0;
        Recyclable::recycle();
    }

    int32_t value = 0;
};

LogHelper helper("fOBJECT_POOL");// NOLINT

int main() {
    auto recycler = Recycler(1, [](){return new Data();});

    auto data1 = recycler.getAs<Data>();
    data1->value = 100;

    auto data2 = recycler.getAs<Data>();
    data2->value = 200;

    data1->recycle();
    data2->recycle();

    return 0;
}