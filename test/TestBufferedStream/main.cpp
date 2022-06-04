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
    char rawBuffer0[5]{};
    auto buffer = BufferedStream(bytes, 4);
    buffer.read(rawBuffer0, 4);
    assert(helper, 0 == std::string("1234").compare(0, 4, rawBuffer0, 0, 4), -1);
    buffer.read(rawBuffer0, 2);
    assert(helper, 0 == std::string("56").compare(0, 2, rawBuffer0, 0, 2), -2);
    buffer.read(rawBuffer0, 4);
    assert(helper, 0 == std::string("7890").compare(0, 4, rawBuffer0, 0, 4), -3);
    buffer.clear();

    // output 测试
    buffer.write("ABC", 3);
    buffer.write("DEFG", 4);
    buffer.write("HIJ", 3);
    buffer.flush();
    char rawBuffer1[11]{};
    bytes->read(rawBuffer1, 10);
    assert(helper, 0 == std::string("ABCDEFGHIJ").compare(0, 10, rawBuffer1, 0, 10), -4);

    return 0;
}