#include "sese/util/InputBufferWrapper.h"
#include "sese/util/OutputBufferWrapper.h"
#include <sese/record/LogHelper.h>
#include "sese/util/Test.h"

using sese::InputBufferWrapper;
using sese::OutputBufferWrapper;
using sese::record::LogHelper;

int main() {
    LogHelper log("TestBufferWrapper");

    char buffer[16]{};
    auto out = OutputBufferWrapper(buffer, 16);
    auto write_len = out.write("Hello World Sese Library", 24);
    assert(log, write_len == 16, -1);

    char temp[24]{};
    auto in = InputBufferWrapper(buffer, 16);
    auto read_len = in.read(temp, 24);
    assert(log, read_len == 16, -2);
    log.debug("temp = %s", temp);

    return 0;
}