#include "ByteBuilder.h"
#include "Util.h"

#define FILTER_TEST_BYTE_BUILDER "fBYTE_BUILDER"

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
    ASSERT(FILTER_TEST_BYTE_BUILDER, data1.i8 == data2.i8)
    ASSERT(FILTER_TEST_BYTE_BUILDER, data1.i16 == data2.i16)
    ASSERT(FILTER_TEST_BYTE_BUILDER, data1.i32 == data2.i32)
    ASSERT(FILTER_TEST_BYTE_BUILDER, data1.i64 == data2.i64)

    buffer->freeCapacity();
    ASSERT(FILTER_TEST_BYTE_BUILDER, buffer->getCapacity() == 10)

    delete buffer;
    return 0;
}