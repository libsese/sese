#include <sese/BufferedStream.h>
#include <sese/record/LogHelper.h>
#include <sese/ByteBuilder.h>
#include <sese/Test.h>

using sese::BufferedStream;
using sese::ByteBuilder;
using sese::LogHelper;
using sese::Test;

LogHelper helper("fBUFFERED");// NOLINT

int main() {
    auto bytes = std::make_shared<ByteBuilder>(8);
    bytes->write("1234567890", 10);

    // input 测试
    char rawBuffer[4];
    auto buffer = BufferedStream(bytes, 4);
    buffer.read(rawBuffer, 4);
    Test::assert(helper, 0 == std::string("1234").compare(0, 4, rawBuffer, 0, 4), -1);
    buffer.read(rawBuffer, 2);
    Test::assert(helper, 0 == std::string("56").compare(0, 2, rawBuffer, 0, 2), -2);
    buffer.read(rawBuffer, 4);
    Test::assert(helper, 0 == std::string("7890").compare(0, 4, rawBuffer, 0, 4), -3);
    buffer.clear();

    // output 测试

    return 0;
}