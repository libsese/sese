#include "sese/Config.h"
#include "sese/util/Endian.h"
#include "sese/util/OutputStream.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"
#include "sese/util/MemoryViewer.h"
#include "sese/util/ByteBuilder.h"

using sese::ByteBuilder;
using sese::MemoryViewer;
using sese::Test;
using sese::record::LogHelper;

LogHelper helper("fENDIAN");

int main() {
    auto builder = std::make_shared<ByteBuilder>(128);

    {
        int16_t i16 = 0x1234;
        int16_t ni16 = ByteSwap16(i16);
        assert(helper, 0x3412 == ni16, 0);

        char buffer16[6]{0};
        MemoryViewer::peer16(builder, &i16);
        builder->read(buffer16, 6);
        helper.debug(" i16: %s", buffer16);

        MemoryViewer::peer16(builder, &ni16);
        builder->read(buffer16, 6);
        helper.debug("ni16: %s", buffer16);
    }

    {
        int32_t i32 = 0x11223344;
        int32_t ni32 = ByteSwap32(i32);
        assert(helper, 0x44332211 == ni32, 0);

        char buffer32[12]{0};
        MemoryViewer::peer32(builder, &i32);
        builder->read(buffer32, 12);
        helper.debug(" i32: %s", buffer32);

        MemoryViewer::peer32(builder, &ni32);
        builder->read(buffer32, 12);
        helper.debug("ni32: %s", buffer32);
    }

    {
        uint64_t i64 = 0x1122334455667788;
        uint64_t ni64 = ByteSwap64(i64);
        assert(helper, 0x8877665544332211 == ni64, 0);

        char buffer64[24]{0};
        MemoryViewer::peer64(builder, &i64);
        builder->read(buffer64, 24);
        helper.debug(" i64: %s", buffer64);

        MemoryViewer::peer64(builder, &ni64);
        builder->read(buffer64, 24);
        helper.debug("ni64: %s", buffer64);
    }

    return 0;
}
