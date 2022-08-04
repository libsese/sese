#include "sese/ByteBuilder.h"
#include "sese/record/LogHelper.h"
#include "sese/Test.h"

using sese::Test;
using sese::Test;

sese::record::LogHelper helper("fBYTE_BUILDER"); // NOLINT

struct Data {
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
};

int main() {
    auto data1 = Data{5, 10, 200, 700};
    auto buffer = new sese::ByteBuilder(sizeof(Data) - 6);
    buffer->write(&data1, sizeof(Data));

    auto data2 = Data{0};
    buffer->read(&data2, sizeof(Data));
    assert(helper, data1.i8 == data2.i8, 0);
    assert(helper, data1.i16 == data2.i16, 0);
    assert(helper, data1.i32 == data2.i32, 0);
    assert(helper, data1.i64 == data2.i64, 0);

    assert(helper, buffer->freeCapacity() == 10, 0);
    assert(helper, buffer->getCapacity() == 1024, 0);

    delete buffer;
    return 0;
}