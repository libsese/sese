#include "Config.h"
#include "Endian.h"
#include "Util.h"

#define FILTER_TEST_ENDIAN "fEndian"

int main() {
    int16_t i16 = 0x1122;
    int16_t ni16 = ByteSwap16(i16);
    ASSERT(FILTER_TEST_ENDIAN, 0x2211 == ni16)

    int32_t i32 = 0x11223344;
    int32_t ni32 = ByteSwap32(i32);
    ASSERT(FILTER_TEST_ENDIAN, 0x44332211 == ni32)

    int64_t i64 = 0x1122334455667788;
    int64_t ni64 = ByteSwap64(i64);
    ASSERT(FILTER_TEST_ENDIAN, 0x8877665544332211 == ni64)

    return 0;
}
